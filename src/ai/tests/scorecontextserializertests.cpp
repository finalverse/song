/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#include <gtest/gtest.h>

#include <QFile>
#include <QJsonDocument>

#include "ai/internal/context/scorecontextserializer.h"

using namespace finalverse::song::ai;

namespace {
ScoreSnapshot goldenSnapshot()
{
    ScoreSnapshot snapshot;
    snapshot.projectRevision = 7;
    snapshot.nativeFormatVersion = 500;
    snapshot.masterScore = true;
    snapshot.projectName = "Golden Song";
    snapshot.workTitle = "Living Score";
    snapshot.notationName = "Full score";
    snapshot.selection.kind = SelectionSnapshotKind::Range;
    snapshot.selection.startTick = RationalSnapshot { 0, 1 };
    snapshot.selection.endTick = RationalSnapshot { 1, 1 };
    snapshot.selection.startStaffIndex = 0;
    snapshot.selection.endStaffIndexExclusive = 1;
    snapshot.selection.elementCount = 2;
    snapshot.selection.sourceNoteCount = 1;
    snapshot.musicalContext.tempoBpm = 120.0;
    snapshot.musicalContext.concertKeyFifths = 0;
    snapshot.musicalContext.meter = RationalSnapshot { 4, 4 };
    snapshot.parts.push_back(PartSnapshot {
            "part-1", "Voice", "voice", "Voice", 1, true, true, false, false
        });
    snapshot.notes.push_back(NoteSnapshot {
            "note-1", { 0, 1 }, { 1, 4 }, 60, 0, 0
        });
    snapshot.lyrics.push_back(LyricSnapshot {
            "lyric-1", "chord-1", { 0, 1 }, { 0, 1 }, 0, "Song"
        });
    snapshot.omittedFields.push_back("project_stable_id:not_available");
    return snapshot;
}
}

TEST(SongScoreContextSerializerTests, MatchesVersionedGoldenFixture)
{
    QFile fixture(QString::fromUtf8(ai_tests_DATA_ROOT) + QStringLiteral("/data/score-context-v1.json"));
    ASSERT_TRUE(fixture.open(QIODevice::ReadOnly));

    const ScoreContextSerializer serializer;
    const std::string serialized = serializer.toJson(goldenSnapshot());
    const QJsonDocument actual = QJsonDocument::fromJson(QByteArray::fromStdString(serialized));
    const QJsonDocument expected = QJsonDocument::fromJson(fixture.readAll());

    ASSERT_FALSE(actual.isNull());
    ASSERT_FALSE(expected.isNull());
    EXPECT_EQ(actual, expected);
    EXPECT_EQ(serializer.toJson(goldenSnapshot()), serialized);
}
