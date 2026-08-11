/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#include "scorecontextserializer.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

using namespace finalverse::song::ai;

namespace {
QJsonObject rationalJson(const RationalSnapshot& rational)
{
    return {
        { "numerator", rational.numerator },
        { "denominator", rational.denominator }
    };
}

QString selectionKindString(SelectionSnapshotKind kind)
{
    switch (kind) {
    case SelectionSnapshotKind::None:
        return QStringLiteral("none");
    case SelectionSnapshotKind::ElementList:
        return QStringLiteral("element_list");
    case SelectionSnapshotKind::Range:
        return QStringLiteral("range");
    }

    return QStringLiteral("none");
}

QJsonObject selectionJson(const SelectionScopeSnapshot& selection)
{
    QJsonObject result {
        { "kind", selectionKindString(selection.kind) },
        { "element_count", static_cast<qint64>(selection.elementCount) },
        { "source_note_count", static_cast<qint64>(selection.sourceNoteCount) }
    };

    if (selection.startTick) {
        result.insert("start_tick", rationalJson(*selection.startTick));
    }
    if (selection.endTick) {
        result.insert("end_tick", rationalJson(*selection.endTick));
    }
    if (selection.startStaffIndex) {
        result.insert("start_staff_index", static_cast<qint64>(*selection.startStaffIndex));
    }
    if (selection.endStaffIndexExclusive) {
        result.insert("end_staff_index_exclusive", static_cast<qint64>(*selection.endStaffIndexExclusive));
    }

    return result;
}

QJsonObject musicalContextJson(const MusicalContextSnapshot& context)
{
    QJsonObject result;
    if (context.tempoBpm) {
        result.insert("tempo_bpm", *context.tempoBpm);
    }
    if (context.concertKeyFifths) {
        result.insert("concert_key_fifths", *context.concertKeyFifths);
    }
    if (context.meter) {
        result.insert("meter", rationalJson(*context.meter));
    }
    return result;
}

QJsonObject partJson(const PartSnapshot& part)
{
    return {
        { "id", QString::fromStdString(part.id) },
        { "name", QString::fromStdString(part.name) },
        { "instrument_id", QString::fromStdString(part.instrumentId) },
        { "instrument_name", QString::fromStdString(part.instrumentName) },
        { "staff_count", static_cast<qint64>(part.staffCount) },
        { "visible", part.visible },
        { "pitched", part.pitched },
        { "tablature", part.tablature },
        { "percussion", part.percussion }
    };
}

QJsonObject noteJson(const NoteSnapshot& note)
{
    return {
        { "element_id", QString::fromStdString(note.elementId) },
        { "tick", rationalJson(note.tick) },
        { "duration", rationalJson(note.duration) },
        { "midi_pitch", note.midiPitch },
        { "staff_index", static_cast<qint64>(note.staffIndex) },
        { "voice", static_cast<qint64>(note.voice) }
    };
}

QJsonObject lyricJson(const LyricSnapshot& lyric)
{
    return {
        { "element_id", QString::fromStdString(lyric.elementId) },
        { "chord_element_id", QString::fromStdString(lyric.chordElementId) },
        { "tick", rationalJson(lyric.tick) },
        { "melisma_duration", rationalJson(lyric.melismaDuration) },
        { "verse", lyric.verse },
        { "text", QString::fromStdString(lyric.text) }
    };
}
}

std::string ScoreContextSerializer::toJson(const ScoreSnapshot& snapshot) const
{
    QJsonArray parts;
    for (const PartSnapshot& part : snapshot.parts) {
        parts.append(partJson(part));
    }

    QJsonArray notes;
    for (const NoteSnapshot& note : snapshot.notes) {
        notes.append(noteJson(note));
    }

    QJsonArray lyrics;
    for (const LyricSnapshot& lyric : snapshot.lyrics) {
        lyrics.append(lyricJson(lyric));
    }

    QJsonArray omittedFields;
    for (const std::string& field : snapshot.omittedFields) {
        omittedFields.append(QString::fromStdString(field));
    }

    const QJsonObject root {
        { "schema", QString::fromStdString(snapshot.schema) },
        { "project_revision", static_cast<qint64>(snapshot.projectRevision) },
        { "native_format_version", snapshot.nativeFormatVersion },
        { "master_score", snapshot.masterScore },
        { "project_name", QString::fromStdString(snapshot.projectName) },
        { "work_title", QString::fromStdString(snapshot.workTitle) },
        { "notation_name", QString::fromStdString(snapshot.notationName) },
        { "selection", selectionJson(snapshot.selection) },
        { "musical_context", musicalContextJson(snapshot.musicalContext) },
        { "parts", parts },
        { "notes", notes },
        { "lyrics", lyrics },
        { "truncated", snapshot.truncated },
        { "omitted_fields", omittedFields }
    };

    return QJsonDocument(root).toJson(QJsonDocument::Compact).toStdString();
}
