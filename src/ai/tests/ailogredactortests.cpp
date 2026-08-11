/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#include <gtest/gtest.h>

#include "ai/internal/security/ailogredactor.h"

using namespace finalverse::song::ai;

TEST(SongAiLogRedactorTests, RedactsHeadersFieldsAndQuotedValues)
{
    const std::string message = "Authorization: Bearer unit-test-value api_key=another-test-value "
                                "{\"access_token\":\"quoted-test-value\"}";
    const std::string redacted = AiLogRedactor::redact(message);

    EXPECT_TRUE(AiLogRedactor::containsSensitiveMaterial(message));
    EXPECT_EQ(redacted.find("unit-test-value"), std::string::npos);
    EXPECT_EQ(redacted.find("another-test-value"), std::string::npos);
    EXPECT_EQ(redacted.find("quoted-test-value"), std::string::npos);
    EXPECT_NE(redacted.find("[REDACTED]"), std::string::npos);
}

TEST(SongAiLogRedactorTests, RedactsUnlabeledCommonTokenShapes)
{
    const std::string modelToken = std::string("s") + "k-" + std::string(24, 'a');
    const std::string sourceControlToken = std::string("g") + "hp_" + std::string(24, 'b');
    const std::string message = "model " + modelToken + " source " + sourceControlToken;
    const std::string redacted = AiLogRedactor::redact(message);

    EXPECT_EQ(redacted.find(modelToken), std::string::npos);
    EXPECT_EQ(redacted.find(sourceControlToken), std::string::npos);
}

TEST(SongAiLogRedactorTests, LeavesOrdinaryDiagnosticsIntact)
{
    const std::string message = "provider=local status=ready context_bytes=128";

    EXPECT_FALSE(AiLogRedactor::containsSensitiveMaterial(message));
    EXPECT_EQ(AiLogRedactor::redact(message), message);
}
