// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#include "platform/aas/mw/log/log_stream.h"

#include "platform/aas/mw/log/detail/thread_local_guard.h"
#include "platform/aas/mw/log/recorder.h"

#include <cstdint>
#include <cstring>

namespace bmw
{
namespace mw
{
namespace log
{

namespace
{

const std::string kDefaultContext = "DFLT";

}  // namespace

LogStream::LogStream(Recorder& recorder,
                     Recorder& fallback_recorder,
                     const LogLevel log_level,
                     const amp::string_view context_id) noexcept
    
    : recorder_{recorder},
      fallback_recorder_{fallback_recorder},
      context_id_(context_id.data() == nullptr ? kDefaultContext : context_id),
      log_level_{log_level}

{
    // Construction fallback handled in log_stream_factory (using here CallRecorder, would give a false impression)
    slot_ = recorder_.StartRecord(context_id_.GetStringView(), log_level_);
}

LogStream::~LogStream() noexcept
{
    if (slot_.has_value()) 
    {
        CallOnRecorder(&Recorder::StopRecord, slot_.value());
    }
}

LogStream::LogStream(LogStream&& other) noexcept
    : recorder_{other.recorder_},
      fallback_recorder_{other.fallback_recorder_},
      slot_{other.slot_},
      context_id_{other.context_id_},
      log_level_{other.log_level_}
{
    // Detach the moved-from log stream from the slot to ensure the slot is only owned by one LogStream.
    other.slot_.reset();

    other.context_id_ = detail::LoggingIdentifier{""};
    other.log_level_ = LogLevel::kOff;
}

void LogStream::Flush() noexcept
{
    if (slot_.has_value()) 
    {
        CallOnRecorder(&Recorder::StopRecord, slot_.value());
    }
    slot_ = CallOnRecorder(&Recorder::StartRecord, context_id_.GetStringView(), log_level_);
}

LogStream& LogStream::Log(const bool value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const std::int8_t value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const std::int16_t value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const std::int32_t value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const std::int64_t value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const std::uint8_t value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const std::uint16_t value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const std::uint32_t value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const std::uint64_t value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const float value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const double value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const LogString value) noexcept
{
    if (value.Data() == nullptr)
    {
        return *this;
    }
    return LogWithRecorder(amp::string_view{value.Data(), value.Size()});
}

LogStream& LogStream::Log(const LogHex8& value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const LogHex16& value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const LogHex32& value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const LogHex64& value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const LogBin8& value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const LogBin16& value) noexcept
{
    return LogWithRecorder(value);
}


/* False positive: LogStream shouldn't be considered as const. It's mandated by the ara::log API. */
LogStream& LogStream::Log(const LogBin32& value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const LogBin64& value) noexcept
{
    return LogWithRecorder(value);
}

/* False positive: LogStream shouldn't be considered as const. It's mandated by the ara::log API. */

LogStream& LogStream::Log(const LogSlog2Message& value) noexcept
{
    return LogWithRecorder(value);
}

template <>
// Log method for `LogRawBuffer` must be templated so that the overload for `LogString` always
// has higher precedence during overload resolution in case the parameter type is string-like.
// 
LogStream& LogStream::Log(const LogRawBuffer& value) noexcept
{
    if (value.data() == nullptr)
    {
        return *this;
    }
    return LogWithRecorder(value);
}

/* False positive: LogStream shouldn't be considered as const. It's mandated by the ara::log API. */

template <typename T>
LogStream& LogStream::LogWithRecorder(const T value) noexcept
{
    if (slot_.has_value())
    {
        CallOnRecorder<void, const SlotHandle&, const T>(&Recorder::Log, slot_.value(), value);
    }
    return *this;
}

// Magic function, to dispatch any recorder function to either the default recorder (if not in logging stack) or the
// fallback recorder if called within the logging stack. #templatemagic
template <typename ReturnValue, typename... ArgsOfFunction, typename... ArgsPassed>
// Checker overly strict, pointer of object not null and only existing functions called (ensure by typeset)
// NOLINTNEXTLINE(bmw-no-pointer-to-member): See above
ReturnValue LogStream::CallOnRecorder(ReturnValue (Recorder::*arbitrary_function)(ArgsOfFunction...) noexcept,
                                      ArgsPassed&&... args) noexcept
{
    if (not detail::ThreadLocalGuard::IsWithingLogging())
    {
        detail::ThreadLocalGuard guard{};
        // Checker overly strict, pointer of object not null and only existing functions called (ensure by typeset)
        // NOLINTNEXTLINE(bmw-no-pointer-to-member): See above
        return (recorder_.*arbitrary_function)(std::forward<ArgsPassed>(args)...);
    }
    else
    {
        // Checker overly strict, pointer of object not null and only existing functions called (ensure by typeset)
        // NOLINTNEXTLINE(bmw-no-pointer-to-member): See above
        return (fallback_recorder_.*arbitrary_function)(std::forward<ArgsPassed>(args)...);
    }
}

}  // namespace log
}  // namespace mw
}  // namespace bmw
