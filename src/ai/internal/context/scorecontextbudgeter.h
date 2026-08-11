/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#pragma once

#include <cstddef>

#include "global/types/ret.h"

#include "scorecontexttypes.h"

namespace finalverse::song::ai {

class ScoreContextBudgeter
{
public:
    muse::Ret fit(ScoreSnapshot& snapshot, std::size_t maxSerializedBytes) const;
};

}
