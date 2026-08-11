/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#pragma once

#include "modularity/ioc.h"

#include "context/iglobalcontext.h"
#include "iscorecontextbuilder.h"

namespace finalverse::song::ai {
class ScoreContextBuilder : public IScoreContextBuilder, public muse::Contextable
{
    muse::ContextInject<mu::context::IGlobalContext> globalContext = { this };

public:
    explicit ScoreContextBuilder(const muse::modularity::ContextPtr& context)
        : muse::Contextable(context)
    {
    }

    muse::RetVal<ScoreContext> build(const ScoreContextRequest& request) const override;
};
}
