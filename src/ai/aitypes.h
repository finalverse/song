/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "async/channel.h"
#include "async/notification.h"
#include "global/types/retval.h"

namespace finalverse::song::ai {
using AiRequestId = std::string;

enum class AiPrivacyMode {
    SelectionOnly,
    Project,
    LocalOnly
};

enum class AiError {
    InvalidRequest = 6000,
    ContextTooLarge,
    NoCompatibleProvider,
    Canceled,
    ProviderFailure,
    NoActiveScore,
    ContextSerializationFailed
};

struct AiRequestRequirements {
    bool text = true;
    bool structuredOutput = false;
    bool audioInput = false;
    bool streaming = false;
};

struct AiProviderCapabilities {
    bool text = false;
    bool structuredOutput = false;
    bool audioInput = false;
    bool streaming = false;
    bool localOnly = false;

    bool supports(const AiRequestRequirements& requirements, AiPrivacyMode privacyMode) const;
};

struct AiRequest {
    static constexpr std::size_t DEFAULT_MAX_CONTEXT_BYTES = 64 * 1024;

    AiRequestId id;
    std::string feature;
    std::string userIntent;
    std::string context;
    AiPrivacyMode privacyMode = AiPrivacyMode::SelectionOnly;
    AiRequestRequirements requirements;
    std::string preferredProviderId;
    std::size_t maxContextBytes = DEFAULT_MAX_CONTEXT_BYTES;
};

struct AiResponseChunk {
    AiRequestId requestId;
    std::size_t sequence = 0;
    std::string content;
};

struct AiResponse {
    AiRequestId requestId;
    std::string providerId;
    std::string modelId;
    std::string content;
    bool deterministic = false;
};

using AiResult = muse::RetVal<AiResponse>;

class AiRequestHandle
{
public:
    explicit AiRequestHandle(AiRequestId requestId = {});

    const AiRequestId& requestId() const;

    bool cancel();
    bool isCanceled() const;
    bool isFinished() const;

    bool sendChunk(AiResponseChunk chunk);
    bool finish(AiResult result);

    muse::async::Notification& canceled();
    muse::async::Channel<AiResponseChunk>& chunkReceived();
    muse::async::Channel<AiResult>& finished();

    std::vector<AiResponseChunk> chunks() const;
    std::optional<AiResult> result() const;

private:
    struct Data;
    std::shared_ptr<Data> m_data;
};
}
