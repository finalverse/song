/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#include <gtest/gtest.h>

#include "ai/internal/modelrouter.h"

using namespace finalverse::song::ai;

namespace {
class FakeProvider : public IAiProvider
{
public:
    FakeProvider(std::string providerId, AiProviderCapabilities providerCapabilities)
        : m_id(std::move(providerId)), m_capabilities(providerCapabilities)
    {
    }

    std::string id() const override
    {
        return m_id;
    }

    AiProviderCapabilities capabilities() const override
    {
        return m_capabilities;
    }

    AiRequestHandle submit(const AiRequest& request) override
    {
        ++submitCount;
        AiRequestHandle handle(request.id);
        handle.finish(AiResult::make_ok(AiResponse {
            request.id, m_id, "fake", "complete", true
        }));
        return handle;
    }

    void cancel(const AiRequestId& requestId) override
    {
        canceledRequestId = requestId;
    }

    int submitCount = 0;
    AiRequestId canceledRequestId;

private:
    std::string m_id;
    AiProviderCapabilities m_capabilities;
};

AiRequest validRequest()
{
    AiRequest request;
    request.id = "router-request";
    request.feature = "assistant.analysis";
    request.userIntent = "Analyze the selection";
    request.context = "bounded context";
    return request;
}
}

TEST(SongModelRouterTests, LocalOnlyPolicyCannotRouteToCloudProvider)
{
    ModelRouter router;
    AiProviderCapabilities cloudCapabilities;
    cloudCapabilities.text = true;

    AiProviderCapabilities localCapabilities = cloudCapabilities;
    localCapabilities.localOnly = true;

    const auto cloud = std::make_shared<FakeProvider>("cloud", cloudCapabilities);
    const auto local = std::make_shared<FakeProvider>("local", localCapabilities);
    ASSERT_TRUE(router.registerProvider(cloud));
    ASSERT_TRUE(router.registerProvider(local));

    AiRequest request = validRequest();
    request.privacyMode = AiPrivacyMode::LocalOnly;
    request.preferredProviderId = "cloud";

    const IAiProviderPtr selected = router.providerFor(request);
    ASSERT_NE(selected, nullptr);
    EXPECT_EQ(selected->id(), "local");
}

TEST(SongModelRouterTests, PreferredCompatibleProviderWins)
{
    ModelRouter router;
    AiProviderCapabilities capabilities;
    capabilities.text = true;

    ASSERT_TRUE(router.registerProvider(std::make_shared<FakeProvider>("first", capabilities)));
    ASSERT_TRUE(router.registerProvider(std::make_shared<FakeProvider>("preferred", capabilities)));

    AiRequest request = validRequest();
    request.preferredProviderId = "preferred";

    ASSERT_NE(router.providerFor(request), nullptr);
    EXPECT_EQ(router.providerFor(request)->id(), "preferred");
}

TEST(SongModelRouterTests, CapabilityMismatchReturnsTerminalFailure)
{
    ModelRouter router;
    AiProviderCapabilities capabilities;
    capabilities.text = true;
    ASSERT_TRUE(router.registerProvider(std::make_shared<FakeProvider>("text-only", capabilities)));

    AiRequest request = validRequest();
    request.requirements.structuredOutput = true;

    const AiRequestHandle handle = router.submit(request);
    ASSERT_TRUE(handle.isFinished());
    ASSERT_TRUE(handle.result().has_value());
    EXPECT_EQ(handle.result()->ret.code(), static_cast<int>(AiError::NoCompatibleProvider));
}

TEST(SongModelRouterTests, ContextBudgetIsEnforcedBeforeProviderSubmission)
{
    ModelRouter router;
    AiProviderCapabilities capabilities;
    capabilities.text = true;
    const auto provider = std::make_shared<FakeProvider>("provider", capabilities);
    ASSERT_TRUE(router.registerProvider(provider));

    AiRequest request = validRequest();
    request.maxContextBytes = 4;

    const AiRequestHandle handle = router.submit(request);
    ASSERT_TRUE(handle.result().has_value());
    EXPECT_EQ(handle.result()->ret.code(), static_cast<int>(AiError::ContextTooLarge));
    EXPECT_EQ(provider->submitCount, 0);
}

TEST(SongModelRouterTests, CancellationIsForwardedWithoutRequestContent)
{
    ModelRouter router;
    AiProviderCapabilities capabilities;
    capabilities.text = true;
    const auto provider = std::make_shared<FakeProvider>("provider", capabilities);
    ASSERT_TRUE(router.registerProvider(provider));

    router.cancel("cancel-me");
    EXPECT_EQ(provider->canceledRequestId, "cancel-me");
}
