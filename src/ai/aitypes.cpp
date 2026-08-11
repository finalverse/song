/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#include "aitypes.h"

#include <atomic>
#include <mutex>
#include <utility>

using namespace finalverse::song::ai;

bool AiProviderCapabilities::supports(const AiRequestRequirements& requirements, AiPrivacyMode privacyMode) const
{
    if (requirements.text && !text) {
        return false;
    }
    if (requirements.structuredOutput && !structuredOutput) {
        return false;
    }
    if (requirements.audioInput && !audioInput) {
        return false;
    }
    if (requirements.streaming && !streaming) {
        return false;
    }
    if (privacyMode == AiPrivacyMode::LocalOnly && !localOnly) {
        return false;
    }

    return true;
}

struct AiRequestHandle::Data {
    explicit Data(AiRequestId requestId)
        : id(std::move(requestId))
    {
    }

    AiRequestId id;
    std::atomic<bool> isCanceled = false;
    std::atomic<bool> isFinished = false;
    muse::async::Notification canceled;
    muse::async::Channel<AiResponseChunk> chunkReceived;
    muse::async::Channel<AiResult> finished;

    mutable std::mutex mutex;
    std::vector<AiResponseChunk> chunks;
    std::optional<AiResult> result;
};

AiRequestHandle::AiRequestHandle(AiRequestId requestId)
    : m_data(std::make_shared<Data>(std::move(requestId)))
{
}

const AiRequestId& AiRequestHandle::requestId() const
{
    return m_data->id;
}

bool AiRequestHandle::cancel()
{
    const AiResult canceledResult = AiResult::make_ret(AiError::Canceled, "AI request canceled");
    {
        std::lock_guard<std::mutex> guard(m_data->mutex);
        if (isCanceled() || isFinished()) {
            return false;
        }

        m_data->isCanceled = true;
        m_data->isFinished = true;
        m_data->result = canceledResult;
    }

    m_data->canceled.notify();
    m_data->finished.send(canceledResult);
    return true;
}

bool AiRequestHandle::isCanceled() const
{
    return m_data->isCanceled;
}

bool AiRequestHandle::isFinished() const
{
    return m_data->isFinished;
}

bool AiRequestHandle::sendChunk(AiResponseChunk chunk)
{
    if (chunk.requestId.empty()) {
        chunk.requestId = requestId();
    }

    {
        std::lock_guard<std::mutex> guard(m_data->mutex);
        if (isCanceled() || isFinished()) {
            return false;
        }
        m_data->chunks.push_back(chunk);
    }

    m_data->chunkReceived.send(chunk);
    return true;
}

bool AiRequestHandle::finish(AiResult result)
{
    if (result.ret && result.val.requestId.empty()) {
        result.val.requestId = requestId();
    }

    {
        std::lock_guard<std::mutex> guard(m_data->mutex);
        if (isFinished()) {
            return false;
        }
        m_data->result = result;
        m_data->isFinished = true;
    }

    m_data->finished.send(result);
    return true;
}

muse::async::Notification& AiRequestHandle::canceled()
{
    return m_data->canceled;
}

muse::async::Channel<AiResponseChunk>& AiRequestHandle::chunkReceived()
{
    return m_data->chunkReceived;
}

muse::async::Channel<AiResult>& AiRequestHandle::finished()
{
    return m_data->finished;
}

std::vector<AiResponseChunk> AiRequestHandle::chunks() const
{
    std::lock_guard<std::mutex> guard(m_data->mutex);
    return m_data->chunks;
}

std::optional<AiResult> AiRequestHandle::result() const
{
    std::lock_guard<std::mutex> guard(m_data->mutex);
    return m_data->result;
}
