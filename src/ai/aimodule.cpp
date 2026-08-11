/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#include "aimodule.h"

#include "modularity/ioc.h"

#include "iaiservice.h"
#include "imodelrouter.h"
#include "internal/aiservice.h"
#include "internal/modelrouter.h"
#include "internal/providers/deterministicprovider.h"

using namespace finalverse::song::ai;
using namespace muse::modularity;

namespace {
const std::string MODULE_NAME("song_ai");
}

std::string AiModule::moduleName() const
{
    return MODULE_NAME;
}

void AiModule::registerExports()
{
    std::shared_ptr<ModelRouter> router = std::make_shared<ModelRouter>();
    router->registerProvider(std::make_shared<DeterministicProvider>());

    m_router = router;
    m_service = std::make_shared<AiService>(m_router);

    globalIoc()->registerExport<IModelRouter>(MODULE_NAME, m_router);
    globalIoc()->registerExport<IAiService>(MODULE_NAME, m_service);
}
