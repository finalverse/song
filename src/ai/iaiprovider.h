/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#pragma once

#include <memory>
#include <string>

#include "aitypes.h"

namespace finalverse::song::ai {
class IAiProvider
{
public:
    virtual ~IAiProvider() = default;

    virtual std::string id() const = 0;
    virtual AiProviderCapabilities capabilities() const = 0;
    virtual AiRequestHandle submit(const AiRequest& request) = 0;
    virtual void cancel(const AiRequestId& requestId) = 0;
};

using IAiProviderPtr = std::shared_ptr<IAiProvider>;
}
