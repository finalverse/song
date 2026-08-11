/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#pragma once

#include <string>
#include <vector>

#include "modularity/imoduleinterface.h"

#include "aitypes.h"
#include "iaiprovider.h"

namespace finalverse::song::ai {

class IModelRouter : MODULE_GLOBAL_INTERFACE
{
    INTERFACE_ID(IModelRouter)

public:
    virtual ~IModelRouter() = default;

    virtual muse::Ret registerProvider(IAiProviderPtr provider) = 0;
    virtual muse::Ret unregisterProvider(const std::string& providerId) = 0;
    virtual std::vector<std::string> providerIds() const = 0;

    virtual IAiProviderPtr providerFor(const AiRequest& request) const = 0;
    virtual AiRequestHandle submit(const AiRequest& request) = 0;
    virtual void cancel(const AiRequestId& requestId) = 0;
};

}
