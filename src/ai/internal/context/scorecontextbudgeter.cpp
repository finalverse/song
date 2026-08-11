/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#include "scorecontextbudgeter.h"

#include <algorithm>
#include <string>

#include "aitypes.h"
#include "scorecontextserializer.h"

using namespace finalverse::song::ai;

namespace {
void addOmission(ScoreSnapshot& snapshot, const std::string& field)
{
    if (std::find(snapshot.omittedFields.begin(), snapshot.omittedFields.end(), field) == snapshot.omittedFields.end()) {
        snapshot.omittedFields.push_back(field);
    }
    snapshot.truncated = true;
}

template<typename T>
void halve(std::vector<T>& values)
{
    values.resize(values.size() / 2);
}
}

muse::Ret ScoreContextBudgeter::fit(ScoreSnapshot& snapshot, std::size_t maxSerializedBytes) const
{
    if (maxSerializedBytes < ScoreContextRequest::MIN_SERIALIZED_BYTES) {
        return muse::make_ret(static_cast<int>(AiError::ContextSerializationFailed),
                              "Score context byte budget is below the safe minimum");
    }

    const ScoreContextSerializer serializer;
    auto fits = [&snapshot, maxSerializedBytes, &serializer]() {
        return serializer.toJson(snapshot).size() <= maxSerializedBytes;
    };

    if (fits()) {
        return muse::make_ok();
    }

    if (!snapshot.lyrics.empty()) {
        addOmission(snapshot, "lyrics:byte_budget");
        while (!snapshot.lyrics.empty() && !fits()) {
            halve(snapshot.lyrics);
        }
    }

    if (!snapshot.notes.empty() && !fits()) {
        addOmission(snapshot, "notes:byte_budget");
        while (!snapshot.notes.empty() && !fits()) {
            halve(snapshot.notes);
        }
    }

    if (!snapshot.parts.empty() && !fits()) {
        addOmission(snapshot, "parts:byte_budget");
        while (!snapshot.parts.empty() && !fits()) {
            halve(snapshot.parts);
        }
    }

    if (!fits()) {
        snapshot.workTitle.clear();
        snapshot.notationName.clear();
        addOmission(snapshot, "optional_titles:byte_budget");
    }

    if (!fits()) {
        return muse::make_ret(static_cast<int>(AiError::ContextSerializationFailed),
                              "Score context metadata cannot fit the requested byte budget");
    }

    return muse::make_ok();
}
