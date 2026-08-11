/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#pragma once

#include "iaiprovider.h"

namespace finalverse::song::ai {

class DeterministicProvider : public IAiProvider
{
public:
    static constexpr const char* PROVIDER_ID = "song.local.deterministic";
    static constexpr const char* MODEL_ID = "song-local-analysis-v1";

    std::string id() const override;
    AiProviderCapabilities capabilities() const override;
    AiRequestHandle submit(const AiRequest& request) override;
    void cancel(const AiRequestId& requestId) override;
};

}
