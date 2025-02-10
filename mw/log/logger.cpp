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


#include "platform/aas/mw/log/logger.h"

#include "platform/aas/mw/log/log_stream_factory.h"
#include "platform/aas/mw/log/runtime.h"

namespace
{
const std::string kDefaultContext = "DFLT";
}

namespace bmw
{
namespace mw
{
namespace log
{

Logger::Logger(const amp::string_view context) noexcept
    : context_(context.data() == nullptr ? GetDefaultContextId() : context)
{
}



log::LogStream Logger::LogFatal() const noexcept
{
    return bmw::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kFatal, context_.GetStringView());
}

log::LogStream Logger::LogError() const noexcept
{
    return bmw::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kError, context_.GetStringView());
}

log::LogStream Logger::LogWarn() const noexcept
{
    return bmw::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kWarn, context_.GetStringView());
}

log::LogStream Logger::LogInfo() const noexcept
{
    return bmw::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kInfo, context_.GetStringView());
}

log::LogStream Logger::LogDebug() const noexcept
{
    return bmw::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kDebug, context_.GetStringView());
}

log::LogStream Logger::LogVerbose() const noexcept
{
    return bmw::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kVerbose, context_.GetStringView());
}

log::LogStream Logger::WithLevel(
    const LogLevel log_level) const noexcept 
{
    return bmw::mw::log::detail::LogStreamFactory::GetStream(log_level, context_.GetStringView());
}

bool Logger::IsLogEnabled(const LogLevel log_level) const noexcept
{
    return IsEnabled(log_level);
}

bool Logger::IsEnabled(const LogLevel log_level) const noexcept
{
    return bmw::mw::log::detail::Runtime::GetRecorder().IsLogEnabled(log_level, context_.GetStringView());
}


amp::string_view Logger::GetContext() const noexcept
{
    return context_.GetStringView();
}




bmw::mw::log::Logger& CreateLogger(const amp::string_view context) noexcept
{
    return bmw::mw::log::detail::Runtime::GetLoggerContainer().GetLogger(context);
}

bmw::mw::log::Logger& CreateLogger(const amp::string_view context_id, const amp::string_view) noexcept
{
    return CreateLogger(context_id);
}

const std::string& GetDefaultContextId() noexcept
{
    return kDefaultContext;
}

}  // namespace log
}  // namespace mw
}  // namespace bmw
