/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#include <gtest/gtest.h>

#include "ai/internal/providers/deterministicprovider.h"

using namespace finalverse::song::ai;

TEST(SongDeterministicProviderTests, IsLocalStreamingAndProducesStableReadOnlyAnalysis)
{
    DeterministicProvider provider;
    const AiProviderCapabilities capabilities = provider.capabilities();

    EXPECT_TRUE(capabilities.text);
    EXPECT_TRUE(capabilities.streaming);
    EXPECT_TRUE(capabilities.localOnly);
    EXPECT_FALSE(capabilities.structuredOutput);
    EXPECT_FALSE(capabilities.audioInput);

    AiRequest request;
    request.id = "local-1";
    request.feature = "assistant.analysis";
    request.userIntent = "Describe the selected music";
    request.context = "private bounded score context";
    request.privacyMode = AiPrivacyMode::LocalOnly;

    const AiRequestHandle handle = provider.submit(request);
    ASSERT_TRUE(handle.isFinished());
    ASSERT_EQ(handle.chunks().size(), 1U);

    const std::optional<AiResult> result = handle.result();
    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result->ret);
    EXPECT_EQ(result->val.providerId, DeterministicProvider::PROVIDER_ID);
    EXPECT_EQ(result->val.modelId, DeterministicProvider::MODEL_ID);
    EXPECT_TRUE(result->val.deterministic);
    EXPECT_EQ(result->val.content.find(request.context), std::string::npos);
    EXPECT_NE(result->val.content.find("No score changes were made"), std::string::npos);
}
