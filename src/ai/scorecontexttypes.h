/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace finalverse::song::ai {

struct RationalSnapshot {
    int numerator = 0;
    int denominator = 1;

    bool operator==(const RationalSnapshot&) const = default;
};

enum class SelectionSnapshotKind {
    None,
    ElementList,
    Range
};

struct SelectionScopeSnapshot {
    SelectionSnapshotKind kind = SelectionSnapshotKind::None;
    std::optional<RationalSnapshot> startTick;
    std::optional<RationalSnapshot> endTick;
    std::optional<std::size_t> startStaffIndex;
    std::optional<std::size_t> endStaffIndexExclusive;
    std::size_t elementCount = 0;
    std::size_t sourceNoteCount = 0;
};

struct MusicalContextSnapshot {
    std::optional<double> tempoBpm;
    std::optional<int> concertKeyFifths;
    std::optional<RationalSnapshot> meter;
};

struct PartSnapshot {
    std::string id;
    std::string name;
    std::string instrumentId;
    std::string instrumentName;
    std::size_t staffCount = 0;
    bool visible = false;
    bool pitched = false;
    bool tablature = false;
    bool percussion = false;
};

struct NoteSnapshot {
    std::string elementId;
    RationalSnapshot tick;
    RationalSnapshot duration;
    int midiPitch = 0;
    std::size_t staffIndex = 0;
    std::size_t voice = 0;
};

struct LyricSnapshot {
    std::string elementId;
    std::string chordElementId;
    RationalSnapshot tick;
    RationalSnapshot melismaDuration;
    int verse = 0;
    std::string text;
};

struct ScoreSnapshot {
    static constexpr const char* SCHEMA = "finalverse.song.score-context.v1";

    std::string schema = SCHEMA;
    std::size_t projectRevision = 0;
    int nativeFormatVersion = 0;
    bool masterScore = false;

    std::string projectName;
    std::string workTitle;
    std::string notationName;

    SelectionScopeSnapshot selection;
    MusicalContextSnapshot musicalContext;
    std::vector<PartSnapshot> parts;
    std::vector<NoteSnapshot> notes;
    std::vector<LyricSnapshot> lyrics;

    bool truncated = false;
    std::vector<std::string> omittedFields;
};

struct ScoreContextRequest {
    static constexpr std::size_t DEFAULT_MAX_SERIALIZED_BYTES = 32 * 1024;
    static constexpr std::size_t MIN_SERIALIZED_BYTES = 4 * 1024;
    static constexpr std::size_t DEFAULT_MAX_NOTE_EVENTS = 512;
    static constexpr std::size_t DEFAULT_MAX_LYRIC_EVENTS = 256;

    bool includeLyrics = true;
    std::size_t maxSerializedBytes = DEFAULT_MAX_SERIALIZED_BYTES;
    std::size_t maxNoteEvents = DEFAULT_MAX_NOTE_EVENTS;
    std::size_t maxLyricEvents = DEFAULT_MAX_LYRIC_EVENTS;
};

struct ScoreContext {
    ScoreSnapshot snapshot;
    std::string serialized;
    std::size_t estimatedTokens = 0;
};

}
