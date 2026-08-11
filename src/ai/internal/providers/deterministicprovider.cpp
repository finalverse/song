/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#include "deterministicprovider.h"

using namespace finalverse::song::ai;

std::string DeterministicProvider::id() const
{
    return PROVIDER_ID;
}

AiProviderCapabilities DeterministicProvider::capabilities() const
{
    AiProviderCapabilities result;
    result.text = true;
    result.streaming = true;
    result.localOnly = true;
    return result;
}

AiRequestHandle DeterministicProvider::submit(const AiRequest& request)
{
    AiRequestHandle handle(request.id);
    if (request.id.empty()) {
        handle.finish(AiResult::make_ret(AiError::InvalidRequest, "AI request identifier is required"));
        return handle;
    }

    const std::string content = "Song local analysis inspected " + std::to_string(request.context.size())
                                + " bytes of bounded score context. No score changes were made.";

    handle.sendChunk(AiResponseChunk { request.id, 0, content });
    handle.finish(AiResult::make_ok(AiResponse {
        request.id,
        PROVIDER_ID,
        MODEL_ID,
        content,
        true
    }));
    return handle;
}

void DeterministicProvider::cancel(const AiRequestId&)
{
    // Requests complete synchronously. Future asynchronous local providers must
    // retain their handles and honor both this method and handle cancellation.
}
