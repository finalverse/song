/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#pragma once

#include "modularity/imoduleinterface.h"
#include "global/types/retval.h"

#include "scorecontexttypes.h"

namespace finalverse::song::ai {

class IScoreContextBuilder : MODULE_CONTEXT_INTERFACE
{
    INTERFACE_ID(IScoreContextBuilder)

public:
    virtual ~IScoreContextBuilder() = default;

    // Build synchronously while the notation is stable. The returned context
    // owns all data and contains no engraving or project pointers.
    virtual muse::RetVal<ScoreContext> build(const ScoreContextRequest& request) const = 0;
};

}
