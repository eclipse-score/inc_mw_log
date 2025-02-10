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


#include "platform/aas/mw/log/logging.h"

#include "platform/aas/mw/log/log_level.h"
#include "platform/aas/mw/log/log_stream_factory.h"
#include "platform/aas/mw/log/recorder.h"
#include "platform/aas/mw/log/runtime.h"

bmw::mw::log::LogStream bmw::mw::log::LogFatal() noexcept
{
    return bmw::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kFatal);
}

bmw::mw::log::LogStream bmw::mw::log::LogError() noexcept
{
    return bmw::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kError);
}

bmw::mw::log::LogStream bmw::mw::log::LogWarn() noexcept
{
    return bmw::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kWarn);
}

bmw::mw::log::LogStream bmw::mw::log::LogInfo() noexcept
{
    return bmw::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kInfo);
}

bmw::mw::log::LogStream bmw::mw::log::LogDebug() noexcept
{
    return bmw::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kDebug);
}

bmw::mw::log::LogStream bmw::mw::log::LogVerbose() noexcept
{
    return bmw::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kVerbose);
}

bmw::mw::log::LogStream bmw::mw::log::LogFatal(const amp::string_view context_id) noexcept
{
    return bmw::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kFatal, context_id);
}

bmw::mw::log::LogStream bmw::mw::log::LogError(const amp::string_view context_id) noexcept
{
    return bmw::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kError, context_id);
}

bmw::mw::log::LogStream bmw::mw::log::LogWarn(const amp::string_view context_id) noexcept
{
    return bmw::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kWarn, context_id);
}

bmw::mw::log::LogStream bmw::mw::log::LogInfo(const amp::string_view context_id) noexcept
{
    return bmw::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kInfo, context_id);
}

bmw::mw::log::LogStream bmw::mw::log::LogDebug(const amp::string_view context_id) noexcept
{
    return bmw::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kDebug, context_id);
}

bmw::mw::log::LogStream bmw::mw::log::LogVerbose(const amp::string_view context_id) noexcept
{
    return bmw::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kVerbose, context_id);
}

bmw::mw::log::Recorder& bmw::mw::log::GetDefaultLogRecorder() noexcept
{
    return bmw::mw::log::detail::Runtime::GetRecorder();
}

void bmw::mw::log::SetLogRecorder(bmw::mw::log::Recorder* const recorder) noexcept
{
    bmw::mw::log::detail::Runtime::SetRecorder(recorder);
}
