/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#include "aiservice.h"

#include <utility>

using namespace finalverse::song::ai;

AiService::AiService(std::shared_ptr<IModelRouter> router)
    : m_router(std::move(router))
{
}

std::vector<std::string> AiService::providerIds() const
{
    return m_router->providerIds();
}

AiRequestHandle AiService::submit(const AiRequest& request)
{
    return m_router->submit(request);
}

void AiService::cancel(const AiRequestId& requestId)
{
    m_router->cancel(requestId);
}
