// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#ifndef PLATFORM_AAS_MW_LOG_DETAIL_CONFIGURATION_H
#define PLATFORM_AAS_MW_LOG_DETAIL_CONFIGURATION_H

#include "platform/aas/mw/log/detail/logging_identifier.h"
#include "platform/aas/mw/log/log_level.h"
#include "platform/aas/mw/log/log_mode.h"

#include "amp_string_view.hpp"

#include <string>
#include <unordered_map>
#include <unordered_set>


namespace bmw

{
namespace mw
{
namespace log
{
namespace detail
{

using ContextLogLevelMap = std::unordered_map<LoggingIdentifier, LogLevel, LoggingIdentifier::HashFunction>;

class Configuration final
{
  public:
    Configuration() = default;

    amp::string_view GetEcuId() const noexcept;
    void SetEcuId(const amp::string_view) noexcept;

    amp::string_view GetAppId() const noexcept;
    void SetAppId(const amp::string_view) noexcept;

    amp::string_view GetAppDescription() const noexcept;
    void SetAppDescription(const amp::string_view) noexcept;

    const std::unordered_set<LogMode>& GetLogMode() const noexcept;
    void SetLogMode(const std::unordered_set<LogMode>&) noexcept;

    amp::string_view GetLogFilePath() const noexcept;
    void SetLogFilePath(const amp::string_view) noexcept;

    LogLevel GetDefaultLogLevel() const noexcept;
    void SetDefaultLogLevel(const LogLevel) noexcept;

    LogLevel GetDefaultConsoleLogLevel() const noexcept;
    void SetDefaultConsoleLogLevel(const LogLevel) noexcept;

    const ContextLogLevelMap& GetContextLogLevel() const noexcept;
    void SetContextLogLevel(const ContextLogLevelMap&) noexcept;

    std::size_t GetStackBufferSize() const noexcept;
    void SetStackBufferSize(const std::size_t stack_buffer_size) noexcept;

    std::size_t GetRingBufferSize() const noexcept;
    void SetRingBufferSize(const std::size_t ring_buffer_size) noexcept;

    bool GetRingBufferOverwriteOnFull() const noexcept;
    void SetRingBufferOverwriteOnFull(const bool) noexcept;

    std::size_t GetNumberOfSlots() const noexcept;
    void SetNumberOfSlots(const std::size_t number_of_slots) noexcept;

    std::size_t GetSlotSizeInBytes() const noexcept;
    void SetSlotSizeInBytes(const std::size_t slot_size_bytes) noexcept;

    void SetDataRouterUid(const std::size_t uid) noexcept;
    std::size_t GetDataRouterUid() const noexcept;

    bool GetDynamicDatarouterIdentifiers() const noexcept;
    void SetDynamicDatarouterIdentifiers(const bool enable_dynamic_identifiers) noexcept;

    /// \brief Returns true if the log level is enabled for the context.
    /// \param use_console_default_level Set to true if threshold for console logging should be considered as default
    /// log level. Otherwise default_log_level_ will be used instead.
    /// \details Uses the threshold from context_log_level map if the map contains the context, otherwise consider the
    /// default threshold.
    bool IsLogLevelEnabled(const LogLevel& log_level,
                           const amp::string_view context,
                           const bool check_for_console = false) const noexcept;

    
  private:
    
    
    
    /// \brief DLT ECU ID, four bytes max.
    LoggingIdentifier ecu_id_{"ECU1"};

    /// \brief DLT ECU ID, four bytes max.
    LoggingIdentifier app_id_{"NONE"};
    

    /// \brief Short description of the application.
    std::string app_description_{""};

    /// \brief Active logging backends/sinks.
    std::unordered_set<LogMode> log_mode_{LogMode::kRemote};

    /// \brief Directory path used for file logging.
    std::string log_file_path_{"/tmp"};

    /// \brief Default log maximum log level.
    LogLevel default_log_level_{LogLevel::kWarn};

    /// \brief Default log maximum log level for the console.
    LogLevel default_console_log_level_{LogLevel::kWarn};

    /// \brief Maximum log level per context.
    ContextLogLevelMap context_log_level_{};

    /// \brief Stack buffer size used for libtracing.
    std::size_t stack_buffer_size_{65536};

    /// \brief Ring buffer size used for libtracing.
    std::size_t ring_buffer_size_{2097152};

    /// \brief Overwrite ring buffer on full for libtracing.
    bool ring_buffer_overwrite_on_full_{true};

    /// \brief Number of Slots for the Datarouter backend.
    std::size_t number_of_slots_{8};

    /// \brief Slot size of each Slot in the Datarouter backend.
    std::size_t slot_size_bytes_{2048};

    /// \brief uid of data router.
    std::size_t data_router_uid_{1038};

    /// \brief Toggle between dynamic datarouter identifiers.
    bool dynamic_datarouter_identifiers_{false};
    
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  // PLATFORM_AAS_MW_LOG_DETAIL_DLT_FORMAT_H
