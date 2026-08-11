/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#pragma once

#include <memory>

#include "iaiservice.h"
#include "imodelrouter.h"

namespace finalverse::song::ai {
class AiService : public IAiService
{
public:
    explicit AiService(std::shared_ptr<IModelRouter> router);

    std::vector<std::string> providerIds() const override;
    AiRequestHandle submit(const AiRequest& request) override;
    void cancel(const AiRequestId& requestId) override;

private:
    std::shared_ptr<IModelRouter> m_router;
};
}
