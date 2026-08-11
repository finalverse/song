/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#include <gtest/gtest.h>

#include "ai/aitypes.h"

using namespace finalverse::song::ai;

TEST(SongAiRequestHandleTests, CopiesShareLifecycleAndTerminalResult)
{
    AiRequestHandle handle("request-1");
    AiRequestHandle copy = handle;

    EXPECT_TRUE(handle.sendChunk(AiResponseChunk { {}, 0, "analysis" }));
    EXPECT_TRUE(copy.finish(AiResult::make_ok(AiResponse {
        {}, "provider", "model", "complete", true
    })));

    EXPECT_TRUE(handle.isFinished());
    ASSERT_EQ(handle.chunks().size(), 1U);
    EXPECT_EQ(handle.chunks().front().requestId, "request-1");

    const std::optional<AiResult> result = handle.result();
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result->ret);
    EXPECT_EQ(result->val.requestId, "request-1");
    EXPECT_EQ(result->val.content, "complete");

    EXPECT_FALSE(handle.sendChunk(AiResponseChunk { {}, 1, "too late" }));
    EXPECT_FALSE(handle.finish(AiResult::make_ret(AiError::ProviderFailure)));
}

TEST(SongAiRequestHandleTests, CancellationIsIdempotentAndTerminal)
{
    AiRequestHandle handle("request-2");

    EXPECT_TRUE(handle.cancel());
    EXPECT_FALSE(handle.cancel());
    EXPECT_TRUE(handle.isCanceled());
    EXPECT_TRUE(handle.isFinished());

    const std::optional<AiResult> result = handle.result();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->ret.code(), static_cast<int>(AiError::Canceled));
    EXPECT_FALSE(handle.sendChunk(AiResponseChunk { {}, 0, "ignored" }));
}

TEST(SongAiRequestHandleTests, CompletedRequestCannotBecomeCanceled)
{
    AiRequestHandle handle("request-3");
    ASSERT_TRUE(handle.finish(AiResult::make_ok(AiResponse {
        {}, "provider", "model", "complete", true
    })));

    EXPECT_FALSE(handle.cancel());
    EXPECT_FALSE(handle.isCanceled());
    ASSERT_TRUE(handle.result().has_value());
    EXPECT_TRUE(handle.result()->ret);
}
