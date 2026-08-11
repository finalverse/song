/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#pragma once

#include <mutex>
#include <vector>

#include "imodelrouter.h"

namespace finalverse::song::ai {

class ModelRouter : public IModelRouter
{
public:
    muse::Ret registerProvider(IAiProviderPtr provider) override;
    muse::Ret unregisterProvider(const std::string& providerId) override;
    std::vector<std::string> providerIds() const override;

    IAiProviderPtr providerFor(const AiRequest& request) const override;
    AiRequestHandle submit(const AiRequest& request) override;
    void cancel(const AiRequestId& requestId) override;

private:
    muse::Ret validate(const AiRequest& request) const;
    std::vector<IAiProviderPtr> providers() const;

    mutable std::mutex m_mutex;
    std::vector<IAiProviderPtr> m_providers;
};

}
