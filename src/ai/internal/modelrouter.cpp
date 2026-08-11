/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#include "modelrouter.h"

#include <algorithm>

using namespace finalverse::song::ai;

muse::Ret ModelRouter::registerProvider(IAiProviderPtr provider)
{
    if (!provider || provider->id().empty()) {
        return muse::make_ret(muse::Ret::Code::BadArgs, std::string("AI providers require a stable identifier"));
    }

    std::lock_guard<std::mutex> guard(m_mutex);
    const auto existing = std::find_if(m_providers.begin(), m_providers.end(), [&provider](const IAiProviderPtr& item) {
        return item->id() == provider->id();
    });

    if (existing != m_providers.end()) {
        *existing = std::move(provider);
    } else {
        m_providers.push_back(std::move(provider));
    }

    return muse::make_ok();
}

muse::Ret ModelRouter::unregisterProvider(const std::string& providerId)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    const auto existing = std::find_if(m_providers.begin(), m_providers.end(), [&providerId](const IAiProviderPtr& item) {
        return item->id() == providerId;
    });

    if (existing == m_providers.end()) {
        return muse::make_ret(muse::Ret::Code::BadArgs, std::string("AI provider is not registered"));
    }

    m_providers.erase(existing);
    return muse::make_ok();
}

std::vector<std::string> ModelRouter::providerIds() const
{
    std::vector<std::string> result;
    for (const IAiProviderPtr& provider : providers()) {
        result.push_back(provider->id());
    }
    return result;
}

IAiProviderPtr ModelRouter::providerFor(const AiRequest& request) const
{
    const std::vector<IAiProviderPtr> availableProviders = providers();

    if (!request.preferredProviderId.empty()) {
        const auto preferred = std::find_if(availableProviders.begin(),
                                            availableProviders.end(), [&request](const IAiProviderPtr& provider) {
            return provider->id() == request.preferredProviderId
                   && provider->capabilities().supports(request.requirements, request.privacyMode);
        });
        if (preferred != availableProviders.end()) {
            return *preferred;
        }
    }

    const auto compatible = std::find_if(availableProviders.begin(), availableProviders.end(), [&request](const IAiProviderPtr& provider) {
        return provider->capabilities().supports(request.requirements, request.privacyMode);
    });

    return compatible == availableProviders.end() ? nullptr : *compatible;
}

AiRequestHandle ModelRouter::submit(const AiRequest& request)
{
    const muse::Ret validation = validate(request);
    if (!validation) {
        AiRequestHandle rejected(request.id);
        rejected.finish(AiResult::make_ret(validation));
        return rejected;
    }

    IAiProviderPtr provider = providerFor(request);
    if (!provider) {
        AiRequestHandle rejected(request.id);
        rejected.finish(AiResult::make_ret(AiError::NoCompatibleProvider,
                                           "No AI provider satisfies the request capabilities and privacy mode"));
        return rejected;
    }

    return provider->submit(request);
}

void ModelRouter::cancel(const AiRequestId& requestId)
{
    for (const IAiProviderPtr& provider : providers()) {
        provider->cancel(requestId);
    }
}

muse::Ret ModelRouter::validate(const AiRequest& request) const
{
    if (request.id.empty()) {
        return muse::make_ret(static_cast<int>(AiError::InvalidRequest), "AI request identifier is required");
    }
    if (request.feature.empty()) {
        return muse::make_ret(static_cast<int>(AiError::InvalidRequest), "AI feature identifier is required");
    }
    if (request.userIntent.empty()) {
        return muse::make_ret(static_cast<int>(AiError::InvalidRequest), "AI user intent is required");
    }
    if (request.context.size() > request.maxContextBytes) {
        return muse::make_ret(static_cast<int>(AiError::ContextTooLarge), "AI request context exceeds its explicit byte budget");
    }

    return muse::make_ok();
}

std::vector<IAiProviderPtr> ModelRouter::providers() const
{
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_providers;
}
