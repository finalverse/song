/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#pragma once

#include <memory>

#include "modularity/imodulesetup.h"

namespace finalverse::song::ai {
class IAiService;
class IModelRouter;

class AiModule : public muse::modularity::IModuleSetup
{
public:
    std::string moduleName() const override;
    void registerExports() override;
    muse::modularity::IContextSetup* newContext(const muse::modularity::ContextPtr& context) const override;

private:
    std::shared_ptr<IModelRouter> m_router;
    std::shared_ptr<IAiService> m_service;
};

class AiModuleContext : public muse::modularity::IContextSetup
{
public:
    explicit AiModuleContext(const muse::modularity::ContextPtr& context)
        : muse::modularity::IContextSetup(context)
    {
    }

    void registerExports() override;
};
}
