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


#include "platform/aas/mw/log/detail/recorder_factory.h"

#include "platform/aas/lib/os/utils/signal_impl.h"
#include "platform/aas/mw/log/detail/composite_recorder.h"
#include "platform/aas/mw/log/detail/data_router/data_router_backend.h"
#include "platform/aas/mw/log/detail/data_router/data_router_message_client_factory_impl.h"
#include "platform/aas/mw/log/detail/data_router/data_router_recorder.h"
#include "platform/aas/mw/log/detail/data_router/message_passing_factory_impl.h"
#include "platform/aas/mw/log/detail/empty_recorder.h"
#include "platform/aas/mw/log/detail/error.h"
#include "platform/aas/mw/log/detail/file_logging/dlt_message_builder.h"
#include "platform/aas/mw/log/detail/file_logging/file_output_backend.h"
#include "platform/aas/mw/log/detail/file_logging/file_recorder.h"
#include "platform/aas/mw/log/detail/file_logging/text_message_builder.h"
#include "platform/aas/mw/log/detail/file_logging/text_recorder.h"
#include "platform/aas/mw/log/detail/initialization_reporter.h"

#ifdef __QNXNTO__
#include "platform/aas/mw/log/detail/slog/slog_backend.h"
#endif

#include <amp_utility.hpp>

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
std::unique_ptr<Backend> CreateConsoleLoggingBackend(const Configuration& config,
                                                     amp::pmr::memory_resource* memory_resource) noexcept
{
    
     * below. */
    auto message_builder = std::make_unique<TextMessageBuilder>(config.GetEcuId());
    auto allocator = std::make_unique<CircularAllocator<LogRecord>>(config.GetNumberOfSlots(),
                                                                    LogRecord{config.GetSlotSizeInBytes()});
    
     * below. */

    
    
    
    return std::make_unique<FileOutputBackend>(std::move(message_builder),
                                               STDOUT_FILENO,
                                               std::move(allocator),
                                               bmw::os::FcntlImpl::Default(memory_resource),
                                               bmw::os::Unistd::Default(memory_resource));
    
    
    
}

#ifdef __QNXNTO__
std::unique_ptr<Backend> CreateSystemBackend(const Configuration& config, amp::pmr::memory_resource* memory_resource)
{
    return std::make_unique<SlogBackend>(config.GetNumberOfSlots(),
                                         LogRecord{config.GetSlotSizeInBytes()},
                                         config.GetAppId(),
                                         bmw::os::qnx::Slog2Impl::Default(memory_resource));
}
#endif


std::unique_ptr<Backend> CreateFileLoggingBackend(const Configuration& config,
                                                  amp::pmr::memory_resource* memory_resource,
                                                  amp::pmr::unique_ptr<bmw::os::Fcntl> fcntl_instance) noexcept

{
    const std::string file_name{std::string(config.GetLogFilePath().data(), config.GetLogFilePath().size()) + "/" +
                                std::string{config.GetAppId().data(), config.GetAppId().size()} + ".dlt"};

    // NOLINTBEGIN(bmw-banned-function): FileLoggingBackend is disabled in production. Argumentation: 
    const auto descriptor_result = fcntl_instance->open(
        file_name.data(),
        bmw::os::Fcntl::Open::kWriteOnly | bmw::os::Fcntl::Open::kCreate | bmw::os::Fcntl::Open::kCloseOnExec,
        bmw::os::Stat::Mode::kReadUser | bmw::os::Stat::Mode::kWriteUser | bmw::os::Stat::Mode::kReadGroup |
            bmw::os::Stat::Mode::kReadOthers);
    // NOLINTEND(bmw-banned-function): see above for detailed explanation

    std::int32_t descriptor{0};
    
    if (descriptor_result.has_value()) 
    
    {
        descriptor = descriptor_result.value();
    }
    else
    {
        
        ReportInitializationError(Error::kLogFileCreationFailed, descriptor_result.error().ToString());
        
        return nullptr;
    }

    
    auto message_builder = std::make_unique<DltMessageBuilder>(config.GetEcuId());
    auto allocator = std::make_unique<CircularAllocator<LogRecord>>(config.GetNumberOfSlots(),
                                                                    LogRecord{config.GetSlotSizeInBytes()});
    

    return std::make_unique<FileOutputBackend>(std::move(message_builder),
                                               descriptor,
                                               std::move(allocator),
                                               bmw::os::Fcntl::Default(memory_resource),
                                               bmw::os::Unistd::Default(memory_resource));
}

}  // namespace



std::unique_ptr<Recorder> RecorderFactory::CreateRecorderFromLogMode(
    const LogMode& log_mode,
    const Configuration& config,
    amp::pmr::unique_ptr<bmw::os::Fcntl> fcntl_instance,
    amp::pmr::memory_resource* memory_resource) const noexcept


{
    if (memory_resource == nullptr)
    {
        ReportInitializationError(bmw::mw::log::detail::Error::kMemoryResourceError);
        return CreateStub();
    }

    switch (log_mode)
    {
        case LogMode::kRemote:
        {
            return GetRemoteRecorder(config, memory_resource);
        }
        case LogMode::kFile:
        {
            return GetFileRecorder(config, std::move(fcntl_instance), memory_resource);
        }
        case LogMode::kConsole:
        {
            return GetConsoleRecorder(config, memory_resource);
        }
        case LogMode::kSystem:
        {
#ifdef __QNXNTO__
            return GetSystemRecorder(config, memory_resource);
#else
            ReportInitializationError(Error::kRecorderFactoryUnsupportedLogMode);
            return std::make_unique<EmptyRecorder>();
#endif
        }
        case LogMode::kInvalid:  // Intentional fall-through
        default:
        {
            ReportInitializationError(Error::kRecorderFactoryUnsupportedLogMode);
            return std::make_unique<EmptyRecorder>();
        }
    }
}

std::unique_ptr<Recorder> RecorderFactory::GetRemoteRecorder(const Configuration& config,
                                                             amp::pmr::memory_resource* memory_resource) noexcept
{
    auto message_client_factory = std::make_unique<DatarouterMessageClientFactoryImpl>(
        config,
        std::make_unique<MessagePassingFactoryImpl>(),
        MsgClientUtils{bmw::os::Unistd::Default(memory_resource),
                       bmw::os::Pthread::Default(memory_resource),
                       amp::pmr::make_unique<bmw::os::SignalImpl>(memory_resource)});
    
    
    WriterFactory::OsalInstances writer_factory_osal = {bmw::os::Fcntl::Default(memory_resource),
                                                        bmw::os::Unistd::Default(memory_resource),
                                                        bmw::os::Mman::Default(memory_resource),
                                                        bmw::os::Stat::Default(memory_resource),
                                                        bmw::os::Stdlib::Default(memory_resource)};

    return std::make_unique<DataRouterRecorder>(
        std::make_unique<DataRouterBackend>(config.GetNumberOfSlots(),
                                            LogRecord{config.GetSlotSizeInBytes()},
                                            *message_client_factory,
                                            config,
                                            WriterFactory{std::move(writer_factory_osal)}),
        config);
    
    
}

std::unique_ptr<Recorder> RecorderFactory::GetFileRecorder(const Configuration& config,
                                                           amp::pmr::unique_ptr<bmw::os::Fcntl> fcntl_instance,
                                                           amp::pmr::memory_resource* memory_resource) noexcept
{
    auto backend = CreateFileLoggingBackend(config, memory_resource, std::move(fcntl_instance));
    if (backend == nullptr)
    {
        return std::make_unique<EmptyRecorder>();
    }
    
    
    return std::make_unique<FileRecorder>(config, std::move(backend));
    
    
}

std::unique_ptr<Recorder> RecorderFactory::GetConsoleRecorder(const Configuration& config,
                                                              amp::pmr::memory_resource* memory_resource) noexcept
{
    auto backend = CreateConsoleLoggingBackend(config, memory_resource);
    constexpr bool check_log_level_for_console = true;
    
    
    return std::make_unique<TextRecorder>(config, std::move(backend), check_log_level_for_console);
    
    
}

#ifdef __QNXNTO__
std::unique_ptr<Recorder> RecorderFactory::GetSystemRecorder(const Configuration& config,
                                                             amp::pmr::memory_resource* memory_resource) noexcept
{
    auto backend = CreateSystemBackend(config, memory_resource);
    constexpr bool check_log_level_for_console = false;
    return std::make_unique<TextRecorder>(config, std::move(backend), check_log_level_for_console);
}
#endif


std::unique_ptr<Recorder> RecorderFactory::CreateFromConfiguration(
    const std::unique_ptr<const ITargetConfigReader> config_reader,
    amp::pmr::memory_resource* memory_resource) const noexcept

{
    if (memory_resource == nullptr)
    {
        ReportInitializationError(bmw::mw::log::detail::Error::kMemoryResourceError);
        return CreateStub();
    }
    const auto result = config_reader->ReadConfig();

    
    if (!result.has_value())
    
    {
        ReportInitializationError(result.error(), "Failed to load configuration files. Fallback to console logging.");
        return CreateWithConsoleLoggingOnly(memory_resource);
    }

    std::vector<std::unique_ptr<Recorder>> recorders;
    
    /* The lambda will be executed within this stack. Thus, all references are still valid */
    amp::ignore =
        std::for_each(result->GetLogMode().begin(),
                      result->GetLogMode().end(),
                      [this, &result, &recorders, &memory_resource](const auto& log_mode) noexcept {
                          amp::ignore = recorders.emplace_back(CreateRecorderFromLogMode(
                              log_mode, result.value(), bmw::os::Fcntl::Default(memory_resource), memory_resource));
                      });
    

    if (recorders.empty())
    {
        ReportInitializationError(Error::kNoLogModeSpecified);
        return std::make_unique<EmptyRecorder>();
    }

    if (recorders.size() == 1U)
    {
        return std::move(recorders[0]);
    }

    
    
    // Composite recorder is needed iff there are more than one activate recorder.
    return std::make_unique<CompositeRecorder>(std::move(recorders));
    
    
}


std::unique_ptr<Recorder> RecorderFactory::CreateWithConsoleLoggingOnly(
    amp::pmr::memory_resource* memory_resource) const noexcept

{
    if (memory_resource == nullptr)
    {
        ReportInitializationError(bmw::mw::log::detail::Error::kMemoryResourceError);
        return CreateStub();
    }
    const Configuration config;  //  using all default values
    auto backend = CreateConsoleLoggingBackend(config, memory_resource);
    constexpr bool check_log_level_for_console = false;
    
    
    return std::make_unique<TextRecorder>(config, std::move(backend), check_log_level_for_console);
    
    
}


std::unique_ptr<Recorder> RecorderFactory::CreateStub() const noexcept

{
    return std::make_unique<EmptyRecorder>();
}


std::unique_ptr<bmw::mw::log::IRecorderFactory> CreateRecorderFactory() noexcept

{
    return std::make_unique<RecorderFactory>();
}


std::unique_ptr<Recorder> RecorderFactory::CreateFromConfiguration(
    amp::pmr::memory_resource* memory_resource) const noexcept

{
    if (memory_resource == nullptr)
    {
        ReportInitializationError(bmw::mw::log::detail::Error::kMemoryResourceError);
        return CreateStub();
    }

    
    return CreateFromConfiguration(std::make_unique<TargetConfigReader>(std::make_unique<ConfigurationFileDiscoverer>(
                                       bmw::os::Path::Default(memory_resource),
                                       bmw::os::Stdlib::Default(memory_resource),
                                       bmw::os::Unistd::Default(memory_resource))),
                                   memory_resource);
    
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
