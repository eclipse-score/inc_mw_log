// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#include "platform/aas/mw/log/legacy_non_verbose_api/tracing.h"

#include "platform/aas/lib/os/unistd.h"

#include <iostream>
#include <sstream>

namespace
{

template <class CharType, class Traits>
// '<<' is not a left shift operator but an overload for logging the respective types.
// code analysis tools tend to assume otherwise hence a false positive in this usecase
// 
auto operator<<(std::basic_ostream<CharType, Traits>& os, amp::string_view sv) -> std::basic_ostream<CharType, Traits>&
{
    os.write(sv.data(), static_cast<std::streamsize>(sv.size()));
    return os;
}

}  // namespace

namespace bmw
{
namespace platform
{

logger::logger(const amp::optional<const bmw::mw::log::detail::Configuration>& config,
               const amp::optional<const bmw::mw::log::NvConfig>& nv_config,
               amp::optional<bmw::mw::log::detail::SharedMemoryWriter>&& writer) noexcept
    
    
    : config_(config.has_value() ? config.value() : bmw::mw::log::detail::Configuration()),
      nvconfig_(nv_config.has_value() ? nv_config.value() : bmw::mw::log::NvConfig()),
      
      
      discard_operation_fallback_shm_data_{},
      discard_operation_fallback_shm_writer_{InitializeSharedData(discard_operation_fallback_shm_data_),
                                             []() noexcept {}},
      appPrefix{}
{
    
    if (writer.has_value())
    
    {
        shared_memory_writer_ = std::move(writer.value());
    }
    constexpr auto idsize = bmw::mw::log::detail::LoggingIdentifier::kMaxLength;
    
    std::fill(appPrefix.begin(), appPrefix.end(), 0);
    auto appPrefixIter = appPrefix.begin();
    static_assert(idsize < std::numeric_limits<int32_t>::max(), "Unsupported length!");
    std::advance(appPrefixIter, static_cast<int32_t>(idsize));
    appPrefixIter = std::copy(config_.GetEcuId().begin(), config_.GetEcuId().end(), appPrefixIter);
    std::ignore = std::copy(config_.GetAppId().begin(), config_.GetAppId().end(), appPrefixIter);

    const auto readResult = nvconfig_.parseFromJson();
    if (readResult != bmw::mw::log::NvConfig::ReadResult::kOK)
    {
        std::cerr << "could not read message ID table for non-verbose DLT!";
        if (readResult == bmw::mw::log::NvConfig::ReadResult::kERROR_PARSE)
        {
            std::cerr << "cannot parse config\n";
        }
        else
        {
            std::cerr << "incompatible content\n";
        }
    }
}

std::optional<LogLevel> logger::GetLevelForContext(const std::string& name) const noexcept
{
    const bmw::mw::log::config::NvMsgDescriptor* const msg_desc = nvconfig_.getDltMsgDesc(name);
    if (msg_desc != nullptr)
    {
        const auto ctxId = msg_desc->ctxid_;
        const auto context_log_level_map = config_.GetContextLogLevel();
        const auto context = context_log_level_map.find(ctxId);
        if (context != context_log_level_map.end())
        {
            return static_cast<LogLevel>(context->second);
        }
    }
    return std::nullopt;
}


logger& logger::instance(const amp::optional<const bmw::mw::log::detail::Configuration>& config,
                         const amp::optional<const bmw::mw::log::NvConfig>& nv_config,
                         amp::optional<bmw::mw::log::detail::SharedMemoryWriter> writer) noexcept

{
    if (*GetInjectedTestInstance() != nullptr)
    {
        return **GetInjectedTestInstance();
    }
    
    // It's a singleton by design hence cannot be made const
    // 
    static logger logger_instance{config, nv_config, std::move(writer)};
    
    return logger_instance;
}

logger** logger::GetInjectedTestInstance()
{
    static logger* pointer{nullptr};
    return &pointer;
}



void logger::InjectTestInstance(logger* const logger_ptr)
{
    *GetInjectedTestInstance() = logger_ptr;
}


const bmw::mw::log::detail::Configuration& logger::get_config() const
{
    return config_;
}

const bmw::mw::log::NvConfig& logger::get_non_verbose_config() const
{
    return nvconfig_;
}

bmw::mw::log::detail::SharedMemoryWriter& logger::GetSharedMemoryWriter()
{
    if (shared_memory_writer_.has_value())
    {
        return shared_memory_writer_.value();
    }
    //  return a fallback that will discard any operations requested as a way of dealing with logging operation
    //  failures. This approach is used to avoid application abort.
    // Using getter method and return the reference to avoid copy overhead.
    // 
    return discard_operation_fallback_shm_writer_;
}


}  // namespace platform
}  // namespace bmw
