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

namespace finalverse::song::ai {
class IAiService : MODULE_GLOBAL_INTERFACE
{
    INTERFACE_ID(IAiService)

public:
    virtual ~IAiService() = default;

    virtual std::vector<std::string> providerIds() const = 0;
    virtual AiRequestHandle submit(const AiRequest& request) = 0;
    virtual void cancel(const AiRequestId& requestId) = 0;
};
}
