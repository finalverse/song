/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#pragma once

#include <string>
#include <string_view>

namespace finalverse::song::ai {

class AiLogRedactor
{
public:
    static std::string redact(std::string_view message);
    static bool containsSensitiveMaterial(std::string_view message);
};

}
