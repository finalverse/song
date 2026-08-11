/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#include "ailogredactor.h"

#include <regex>

using namespace finalverse::song::ai;

namespace {
const std::string REDACTED = "[REDACTED]";

std::string applyRedactionPatterns(std::string value)
{
    static const std::regex authorizationHeader(
        R"((Authorization\s*:\s*(?:Bearer|Basic)\s+)[^\s,;]+)",
        std::regex::icase);
    static const std::regex credentialField(
        R"(((?:api[_-]?key|access[_-]?token|auth[_-]?token|token|secret)\s*[=:]\s*)[^\s"'&,;]+)",
        std::regex::icase);
    static const std::regex quotedCredentialField(
        R"((["'](?:api[_-]?key|access[_-]?token|auth[_-]?token|token|secret)["']\s*:\s*["'])[^"']+(["']))",
        std::regex::icase);
    static const std::regex commonModelToken(R"(\b(?:sk|rk)-[A-Za-z0-9_-]{16,}\b)");
    static const std::regex githubToken(R"(\b(?:gh[pousr]_[A-Za-z0-9]{20,}|github_pat_[A-Za-z0-9_]{20,})\b)");
    static const std::regex awsAccessKey(R"(\bAKIA[0-9A-Z]{16}\b)");

    value = std::regex_replace(value, authorizationHeader, "$1" + REDACTED);
    value = std::regex_replace(value, quotedCredentialField, "$1" + REDACTED + "$2");
    value = std::regex_replace(value, credentialField, "$1" + REDACTED);
    value = std::regex_replace(value, commonModelToken, REDACTED);
    value = std::regex_replace(value, githubToken, REDACTED);
    value = std::regex_replace(value, awsAccessKey, REDACTED);
    return value;
}
}

std::string AiLogRedactor::redact(std::string_view message)
{
    return applyRedactionPatterns(std::string(message));
}

bool AiLogRedactor::containsSensitiveMaterial(std::string_view message)
{
    const std::string original(message);
    return applyRedactionPatterns(original) != original;
}
