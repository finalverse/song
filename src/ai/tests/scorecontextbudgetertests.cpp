/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#include <gtest/gtest.h>

#include "ai/aitypes.h"
#include "ai/internal/context/scorecontextbudgeter.h"
#include "ai/internal/context/scorecontextserializer.h"

using namespace finalverse::song::ai;

TEST(SongScoreContextBudgeterTests, PreservesContextThatAlreadyFits)
{
    ScoreSnapshot snapshot;
    snapshot.projectName = "Small score";
    snapshot.notes.push_back(NoteSnapshot {
        "note-1", { 0, 1 }, { 1, 4 }, 60, 0, 0
    });

    const ScoreContextSerializer serializer;
    const std::string before = serializer.toJson(snapshot);

    const ScoreContextBudgeter budgeter;
    const muse::Ret result = budgeter.fit(snapshot, ScoreContextRequest::DEFAULT_MAX_SERIALIZED_BYTES);

    ASSERT_TRUE(result);
    EXPECT_FALSE(snapshot.truncated);
    EXPECT_TRUE(snapshot.omittedFields.empty());
    EXPECT_EQ(serializer.toJson(snapshot), before);
}

TEST(SongScoreContextBudgeterTests, DropsDetailedEventsUntilSerializedContextFits)
{
    ScoreSnapshot snapshot;
    snapshot.projectName = "Budget test";

    for (int index = 0; index < 100; ++index) {
        snapshot.notes.push_back(NoteSnapshot {
            "note-" + std::to_string(index),
            { index, 4 },
            { 1, 4 },
            60 + (index % 12),
            static_cast<std::size_t>(index % 4),
            static_cast<std::size_t>(index % 2)
        });
        snapshot.lyrics.push_back(LyricSnapshot {
            "lyric-" + std::to_string(index),
            "chord-" + std::to_string(index),
            { index, 4 },
            { 0, 1 },
            0,
            "bounded lyric text"
        });
    }

    const ScoreContextSerializer serializer;
    ASSERT_GT(serializer.toJson(snapshot).size(), ScoreContextRequest::MIN_SERIALIZED_BYTES);

    const ScoreContextBudgeter budgeter;
    const muse::Ret result = budgeter.fit(snapshot, ScoreContextRequest::MIN_SERIALIZED_BYTES);

    ASSERT_TRUE(result);
    EXPECT_TRUE(snapshot.truncated);
    EXPECT_LE(serializer.toJson(snapshot).size(), ScoreContextRequest::MIN_SERIALIZED_BYTES);
    EXPECT_FALSE(snapshot.omittedFields.empty());
}

TEST(SongScoreContextBudgeterTests, RejectsUnsafeTinyBudget)
{
    ScoreSnapshot snapshot;
    const ScoreContextBudgeter budgeter;

    const muse::Ret result = budgeter.fit(snapshot, ScoreContextRequest::MIN_SERIALIZED_BYTES - 1);

    EXPECT_FALSE(result);
    EXPECT_EQ(result.code(), static_cast<int>(AiError::ContextSerializationFailed));
}
