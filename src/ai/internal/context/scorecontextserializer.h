/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#pragma once

#include <string>

#include "scorecontexttypes.h"

namespace finalverse::song::ai {
class ScoreContextSerializer
{
public:
    std::string toJson(const ScoreSnapshot& snapshot) const;
};
}
