/********************************************************************************
* Copyright (c) 2025 Contributors to the Eclipse Foundation
*
* See the NOTICE file(s) distributed with this work for additional
* information regarding copyright ownership.
*
* This program and the accompanying materials are made available under the
* terms of the Apache License Version 2.0 which is available at
* https://www.apache.org/licenses/LICENSE-2.0
*
* SPDX-License-Identifier: Apache-2.0
********************************************************************************/



#include "platform/aas/mw/log/detail/log_record.h"

#include "amp_utility.hpp"

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

namespace
{

void SetupBuffer(LogRecord& dst, const std::size_t capacity) noexcept
{
    // Beware that std::vector move assignment only preserves content, not capacity.
    dst.getLogEntry().payload.shrink_to_fit();
    dst.getLogEntry().payload.reserve(capacity);

    // Finally update the reference inside verbosePayload to point to the updated payload buffer.
    dst.getVerbosePayload().SetBuffer(dst.getLogEntry().payload);
}

LogRecord& MoveConstruct(LogRecord& dst, LogRecord& src) noexcept
{
    // Get the capacity before we move from src.
    const auto capacity = src.getLogEntry().payload.capacity();

    // Beware of the lifetimes and **assignment order**: VerbosePayload contains a reference to
    // logEntry.verbosePayload_ that would be dangling when logEntry_ is assigned. Thus we update verbosePayload_ first,
    // and then logEntry_ to rule out any undefined behavior.
    dst.getVerbosePayload() = src.getVerbosePayload();
    dst.getLogEntry() = std::move(src.getLogEntry());

    SetupBuffer(dst, capacity);

    return dst;
}

LogRecord& CopyConstruct(LogRecord& dst, const LogRecord& src) noexcept
{
    // Beware of the lifetimes and **assignment order**: VerbosePayload contains a reference to
    // logEntry.verbosePayload_ that would be dangling when logEntry_ is assigned. Thus we update verbosePayload_ first,
    // and then logEntry_ to rule out any undefined behavior.
    dst.getVerbosePayload() = src.getVerbosePayload();
    dst.getLogEntry() = src.getLogEntry();

    SetupBuffer(dst, src.getLogEntry().payload.capacity());

    return dst;
}

}  // namespace

// This is false positive. Constructor is declared only once.
// 
LogRecord::LogRecord(const std::size_t max_payload_size_bytes) noexcept
    : logEntry_{}, verbosePayload_(max_payload_size_bytes, logEntry_.payload)
{
}

LogEntry& LogRecord::getLogEntry() noexcept
{
    // Returning address of non-static class member is justified by design
    // 
    return logEntry_;
}

detail::VerbosePayload& LogRecord::getVerbosePayload() noexcept
{
    // Returning address of non-static class member is justified by design
    // 
    return verbosePayload_;
}

const LogEntry& LogRecord::getLogEntry() const noexcept
{
    return logEntry_;
}

const detail::VerbosePayload& LogRecord::getVerbosePayload() const noexcept
{
    return verbosePayload_;
}

LogRecord::LogRecord(const LogRecord& other) noexcept
    : logEntry_{other.logEntry_}, verbosePayload_{other.verbosePayload_}
{
    amp::ignore = CopyConstruct(*this, other);
}

// a12_8_4 - Conflict with clang_tidy warning:
// std::move of the expression of the trivially-copyable type 'detail::VerbosePayload' has no effect.
// a3_1_1 - This is false positive. Constructor is declared only once.
// 
// 
LogRecord::LogRecord(LogRecord&& other) noexcept : logEntry_{}, verbosePayload_{other.verbosePayload_}
{
    amp::ignore = MoveConstruct(*this, other);
}

LogRecord& LogRecord::operator=(const LogRecord& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }
    amp::ignore = CopyConstruct(*this, other);
    return *this;
}

LogRecord& LogRecord::operator=(LogRecord&& other) noexcept
{
    amp::ignore = MoveConstruct(*this, other);
    return *this;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
