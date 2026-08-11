/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#include "scorecontextbuilder.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <unordered_set>
#include <vector>

#include <QString>

#include "async/notifylist.h"

#include "notation/imasternotation.h"
#include "notation/inotation.h"
#include "notation/inotationelements.h"
#include "notation/inotationinteraction.h"
#include "notation/inotationparts.h"
#include "notation/inotationselection.h"
#include "notation/inotationselectionrange.h"
#include "notation/inotationundostack.h"

#include "engraving/dom/chord.h"
#include "engraving/dom/lyrics.h"
#include "engraving/dom/note.h"
#include "engraving/dom/part.h"
#include "engraving/dom/score.h"
#include "engraving/dom/staff.h"
#include "engraving/dom/timesig.h"

#include "scorecontextbudgeter.h"
#include "scorecontextserializer.h"

using namespace finalverse::song::ai;

namespace {
constexpr qsizetype MAX_TEXT_CHARACTERS = 256;

RationalSnapshot rational(const mu::engraving::Fraction& value)
{
    return { value.numerator(), value.denominator() };
}

std::string limited(const QString& value)
{
    return value.left(MAX_TEXT_CHARACTERS).toStdString();
}

std::string limited(const muse::String& value)
{
    return limited(value.toQString());
}

std::string elementId(const mu::engraving::EngravingObject* element)
{
    if (!element || !element->eid().isValid()) {
        return {};
    }
    return element->eid().toStdString();
}

void addBuilderOmission(ScoreSnapshot& snapshot, const std::string& field)
{
    if (std::find(snapshot.omittedFields.begin(), snapshot.omittedFields.end(), field) == snapshot.omittedFields.end()) {
        snapshot.omittedFields.push_back(field);
    }
}

bool noteLess(const mu::engraving::Note* left, const mu::engraving::Note* right)
{
    if (left->tick() != right->tick()) {
        return left->tick() < right->tick();
    }
    if (left->staffIdx() != right->staffIdx()) {
        return left->staffIdx() < right->staffIdx();
    }
    if (left->voice() != right->voice()) {
        return left->voice() < right->voice();
    }
    if (left->pitch() != right->pitch()) {
        return left->pitch() < right->pitch();
    }
    return elementId(left) < elementId(right);
}

bool lyricLess(const LyricSnapshot& left, const LyricSnapshot& right)
{
    if (left.tick != right.tick) {
        return left.tick.numerator * right.tick.denominator < right.tick.numerator * left.tick.denominator;
    }
    if (left.verse != right.verse) {
        return left.verse < right.verse;
    }
    return left.elementId < right.elementId;
}
}

muse::RetVal<ScoreContext> ScoreContextBuilder::build(const ScoreContextRequest& request) const
{
    if (request.maxSerializedBytes < ScoreContextRequest::MIN_SERIALIZED_BYTES) {
        return muse::RetVal<ScoreContext>::make_ret(AiError::ContextSerializationFailed,
                                                    "Score context byte budget is below the safe minimum");
    }

    const auto applicationContext = globalContext();
    mu::notation::INotationPtr notation = applicationContext ? applicationContext->currentNotation() : nullptr;
    if (!notation || !notation->elements() || !notation->elements()->msScore()) {
        return muse::RetVal<ScoreContext>::make_ret(AiError::NoActiveScore, "No active score is available for AI context");
    }

    mu::engraving::Score* score = notation->elements()->msScore();
    ScoreSnapshot snapshot;
    snapshot.masterScore = notation->isMaster();
    snapshot.projectName = limited(notation->projectName());
    snapshot.workTitle = limited(notation->workTitle());
    snapshot.notationName = limited(notation->name());
    snapshot.projectRevision = notation->undoStack() ? notation->undoStack()->currentStateIndex() : 0;
    snapshot.nativeFormatVersion = notation->masterNotation() ? notation->masterNotation()->mscVersion() : 0;
    addBuilderOmission(snapshot, "project_stable_id:not_available");

    mu::notation::INotationSelectionPtr selection;
    if (notation->interaction()) {
        selection = notation->interaction()->selection();
    }

    mu::engraving::Fraction contextTick(0, 1);
    std::size_t contextStaffIndex = 0;
    std::vector<mu::engraving::Note*> selectedNotes;

    if (!selection || selection->isNone()) {
        snapshot.selection.kind = SelectionSnapshotKind::None;
        addBuilderOmission(snapshot, "notes:no_selection");
    } else {
        snapshot.selection.kind = selection->isRange() ? SelectionSnapshotKind::Range : SelectionSnapshotKind::ElementList;
        snapshot.selection.elementCount = selection->elements().size();

        if (selection->isRange() && selection->range()) {
            const mu::notation::INotationSelectionRangePtr range = selection->range();
            contextTick = range->startTick();
            contextStaffIndex = range->startStaffIndex();
            snapshot.selection.startTick = rational(range->startTick());
            snapshot.selection.endTick = rational(range->endTick());
            snapshot.selection.startStaffIndex = range->startStaffIndex();
            snapshot.selection.endStaffIndexExclusive = range->endStaffIndex();
        }

        std::unordered_set<const mu::engraving::Note*> seenNotes;
        for (mu::engraving::Note* note : selection->notes()) {
            if (note && seenNotes.insert(note).second) {
                selectedNotes.push_back(note);
            }
        }
        std::sort(selectedNotes.begin(), selectedNotes.end(), noteLess);
        snapshot.selection.sourceNoteCount = selectedNotes.size();
        if (snapshot.selection.elementCount == 0) {
            snapshot.selection.elementCount = selectedNotes.size();
        }

        if (!selectedNotes.empty() && !snapshot.selection.startTick) {
            contextTick = selectedNotes.front()->tick();
            contextStaffIndex = selectedNotes.front()->staffIdx();
            snapshot.selection.startTick = rational(contextTick);

            const mu::engraving::Note* last = selectedNotes.back();
            const mu::engraving::Fraction endTick = last->chord() ? last->chord()->endTick() : last->tick();
            snapshot.selection.endTick = rational(endTick);
            snapshot.selection.startStaffIndex = contextStaffIndex;
            snapshot.selection.endStaffIndexExclusive = contextStaffIndex + 1;
        }
    }

    if (selectedNotes.size() > request.maxNoteEvents) {
        selectedNotes.resize(request.maxNoteEvents);
        snapshot.truncated = true;
        addBuilderOmission(snapshot, "notes:event_limit");
    }

    std::unordered_set<const mu::engraving::Chord*> lyricChords;
    for (const mu::engraving::Note* note : selectedNotes) {
        const mu::engraving::Chord* chord = note->chord();
        const mu::engraving::Fraction duration = chord ? chord->actualTicks() : mu::engraving::Fraction(0, 1);
        snapshot.notes.push_back(NoteSnapshot {
            elementId(note),
            rational(note->tick()),
            rational(duration),
            note->pitch(),
            note->staffIdx(),
            note->voice()
        });

        if (!request.includeLyrics || !chord || !lyricChords.insert(chord).second) {
            continue;
        }

        for (const mu::engraving::Lyrics* lyric : chord->lyrics()) {
            if (!lyric || lyric->plainText().isEmpty()) {
                continue;
            }
            snapshot.lyrics.push_back(LyricSnapshot {
                elementId(lyric),
                elementId(chord),
                rational(chord->tick()),
                rational(lyric->ticks()),
                lyric->verse(),
                limited(lyric->plainText())
            });
        }
    }

    if (!request.includeLyrics) {
        addBuilderOmission(snapshot, "lyrics:disabled_by_request");
    }
    std::sort(snapshot.lyrics.begin(), snapshot.lyrics.end(), lyricLess);
    if (snapshot.lyrics.size() > request.maxLyricEvents) {
        snapshot.lyrics.resize(request.maxLyricEvents);
        snapshot.truncated = true;
        addBuilderOmission(snapshot, "lyrics:event_limit");
    }

    if (notation->parts()) {
        for (const mu::notation::Part* part : notation->parts()->partList()) {
            if (!part) {
                continue;
            }
            snapshot.parts.push_back(PartSnapshot {
                part->id().toStdString(),
                limited(part->partName()),
                limited(part->instrumentId(contextTick)),
                limited(part->instrumentName(contextTick)),
                part->nstaves(),
                part->show(),
                part->hasPitchedStaff(),
                part->hasTabStaff(),
                part->hasDrumStaff()
            });
        }
    }

    snapshot.musicalContext.tempoBpm = score->tempo(contextTick).toBPM().val;
    if (mu::engraving::Staff* staff = score->staff(contextStaffIndex)) {
        snapshot.musicalContext.concertKeyFifths = static_cast<int>(staff->concertKey(contextTick));
        if (const mu::engraving::TimeSig* timeSignature = staff->timeSig(contextTick)) {
            snapshot.musicalContext.meter = rational(timeSignature->sig());
        } else {
            addBuilderOmission(snapshot, "musical_context.meter:not_available");
        }
    } else {
        addBuilderOmission(snapshot, "musical_context.key_and_meter:not_available");
    }

    const ScoreContextBudgeter budgeter;
    const muse::Ret budgetResult = budgeter.fit(snapshot, request.maxSerializedBytes);
    if (!budgetResult) {
        return muse::RetVal<ScoreContext>::make_ret(budgetResult);
    }

    const ScoreContextSerializer serializer;
    ScoreContext context;
    context.snapshot = std::move(snapshot);
    context.serialized = serializer.toJson(context.snapshot);
    context.estimatedTokens = (context.serialized.size() + 3) / 4;
    return muse::RetVal<ScoreContext>::make_ok(context);
}
