// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#ifndef AAS_MW_LOG_LEGACY_NON_VERBOSE_API_TRACING_H_
#define AAS_MW_LOG_LEGACY_NON_VERBOSE_API_TRACING_H_

/// \file This file contains the legacy API for non-verbose logging.
/// We only keep this file for legacy compatibility reasons.
/// Going forward a proper C++ API for mw::log shall be defined to replace this code.

#include "serialization/for_logging.h"

#include "platform/aas/lib/os/utils/high_resolution_steady_clock.h"
#include "platform/aas/mw/log/configuration/configuration.h"
#include "platform/aas/mw/log/configuration/nvconfig.h"
#include "platform/aas/mw/log/detail/data_router/shared_memory/writer_factory.h"
#include "platform/aas/mw/log/detail/logging_identifier.h"
#include "platform/aas/mw/log/runtime.h"

#include "amp_utility.hpp"

namespace bmw
{
namespace platform
{

using timestamp_t = bmw::os::HighResolutionSteadyClock::time_point;
using msgsize_t = uint16_t;

enum class LogLevel : uint8_t
{
    kOff = 0x00,
    kFatal = 0x01,
    kError = 0x02,
    kWarn = 0x03,
    kInfo = 0x04,
    kDebug = 0x05,
    kVerbose = 0x06
};

class logger
{
  public:
    using AppPrefix = std::array<char, bmw::mw::log::detail::LoggingIdentifier::kMaxLength * 3UL>;

    static logger& instance(const amp::optional<const bmw::mw::log::detail::Configuration>& config = amp::nullopt,
                            const amp::optional<const bmw::mw::log::NvConfig>& nv_config = amp::nullopt,
                            amp::optional<bmw::mw::log::detail::SharedMemoryWriter> = amp::nullopt) noexcept;

    explicit logger(const amp::optional<const bmw::mw::log::detail::Configuration>& config,
                    const amp::optional<const bmw::mw::log::NvConfig>& nv_config,
                    amp::optional<bmw::mw::log::detail::SharedMemoryWriter>&&) noexcept;

    template <typename T>
    amp::optional<bmw::mw::log::detail::TypeIdentifier> RegisterType() noexcept
    {
        class TypeinfoWithPrefix
        {
          public:
            explicit TypeinfoWithPrefix(const AppPrefix& appPref) : appPrefix_(appPref) {}
            
            std::size_t size() const
            
            {
                // coverity issue solved with this comparision and to deeply analyze task we created issue.
                // Issue: 
                if (::bmw::common::visitor::logger_type_info<T>().size() >
                    std::numeric_limits<size_t>::max() - appPrefix_.size())
                {
                    // If an overflow were to happen, then its safe to return
                    // maxpayload size and appPrefixsize.
                    return appPrefix_.size() + bmw::mw::log::detail::SharedMemoryWriter::GetMaxPayloadSize();
                }

                return appPrefix_.size() + ::bmw::common::visitor::logger_type_info<T>().size();
            }
            
            
            void copy(const amp::span<bmw::mw::log::detail::Byte> data) const
            
            
            {
                using SpanSizeType = amp::span<bmw::mw::log::detail::Byte>::size_type;
                std::ignore = std::copy(appPrefix_.begin(), appPrefix_.end(), data.data());
                //  static cast is allowed as size of the span is not expected to have a negative value
                const auto size = static_cast<std::uint64_t>(data.size());
                static_assert(sizeof(size) >= sizeof(SpanSizeType),
                              "Cast to unsigned of at least the same size is expected");
                // ----- COMMON_ARGUMENTATION ----
                // Pointer is needed to iterate char array.
                // --------------------------------
                
                // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) COMMON_ARGUMENTATION.
                ::bmw::common::visitor::logger_type_info<T>().copy(
                    data.subspan(static_cast<std::int64_t>(appPrefix_.size())).data(), size - appPrefix_.size());
                
            }

            
          private:
            
            
            const AppPrefix& appPrefix_;
            
        };

        if (shared_memory_writer_.has_value())
        {
            auto id = shared_memory_writer_.value().TryRegisterType(TypeinfoWithPrefix(appPrefix));
            if (true == id.has_value())
            {
                return static_cast<bmw::mw::log::detail::TypeIdentifier>(id.value());
            }
        }

        return {};
    }

    template <typename T>
    LogLevel get_type_level() const
    {
        auto log_level = LogLevel::kInfo;
        const bmw::mw::log::config::NvMsgDescriptor* const msg_desc =
            nvconfig_.getDltMsgDesc(::bmw::common::visitor::struct_visitable<T>::name());
        if (msg_desc != nullptr)
        {
            auto message_descriptor_log_level = msg_desc->logLevel_;
            // Check the range's value of the log level before casting it to the LogLevel enum.
            if (message_descriptor_log_level <= bmw::mw::log::LogLevel::kVerbose)
            {
                log_level = static_cast<LogLevel>(msg_desc->logLevel_);
            }
        }
        return log_level;
    }

    template <typename T>
    LogLevel get_type_threshold() const noexcept
    {
        return GetLevelForContext(::bmw::common::visitor::struct_visitable<T>::name()).value_or(LogLevel::kVerbose);
    }

    bmw::mw::log::detail::SharedMemoryWriter& GetSharedMemoryWriter();

    const bmw::mw::log::detail::Configuration& get_config() const;
    const bmw::mw::log::NvConfig& get_non_verbose_config() const;

    /// \brief Only for testing to inject an instance to intercept and check the behavior.
    static void InjectTestInstance(logger* const logger_ptr);

    
  private:
    
    static logger** GetInjectedTestInstance();
    std::optional<LogLevel> GetLevelForContext(const std::string& name) const noexcept;

    
    bmw::mw::log::detail::Configuration config_;
    bmw::mw::log::NvConfig nvconfig_;
    amp::optional<bmw::mw::log::detail::SharedMemoryWriter> shared_memory_writer_;
    bmw::mw::log::detail::SharedData discard_operation_fallback_shm_data_;
    bmw::mw::log::detail::SharedMemoryWriter discard_operation_fallback_shm_writer_;
    AppPrefix appPrefix;
    
};

template <typename T>
class log_entry
{
  public:
    static log_entry& instance() noexcept
    {
        // It's a singleton by design hence cannot be made const
        // 
        static log_entry entry{};
        return entry;
    }

    amp::optional<bmw::mw::log::detail::TypeIdentifier> RegisterTypeGetId() noexcept
    {
        const auto registered_id = logger::instance().RegisterType<T>();
        if (registered_id.has_value())
        {
            shared_memory_id_ = registered_id.value();
        }
        return registered_id;
    }

    template <typename F>
    void TrySerializeIntoSharedMemory(F serialize) noexcept
    {
        if (bmw::mw::log::detail::GetRegisterTypeToken() == shared_memory_id_)
        {
            if (!RegisterTypeGetId().has_value())
            {
                return;
            }
        }
        serialize();
    }

    
    void TryWriteIntoSharedMemory(const T& t) noexcept
    
    {
        TrySerializeIntoSharedMemory(
            
            [&t, this]() noexcept
            
            {
                using s = ::bmw::common::visitor::logging_serializer;
                logger::instance().GetSharedMemoryWriter().AllocAndWrite(
                    
                    [&t]
                    
                    (const auto data_span) {
                        return s::serialize(t, data_span.data(), bmw::mw::log::detail::GetDataSizeAsLength(data_span));
                    },
                    shared_memory_id_,
                    static_cast<uint64_t>(s::serialize_size(t)));
            });
    }

    
    /// \public
    /// \thread-safe
    void log_at_time(timestamp_t timestamp, const T& t)
    
    {
        TrySerializeIntoSharedMemory(
            
            [&timestamp, &t, this]()
            
            {
                using s = ::bmw::common::visitor::logging_serializer;
                logger::instance().GetSharedMemoryWriter().AllocAndWrite(
                    timestamp,
                    shared_memory_id_,
                    s::serialize_size(t),
                    
                    [&t](const auto data_span)
                    
                    {
                        return s::serialize(t, data_span.data(), bmw::mw::log::detail::GetDataSizeAsLength(data_span));
                    });
            });
    }

    log_entry() noexcept
        : shared_memory_id_{bmw::mw::log::detail::GetRegisterTypeToken()},
          default_enabled_{false},
          level_enabled_{LogLevel::kVerbose}
    {
        amp::ignore = bmw::mw::log::detail::Runtime::GetRecorder();
        
        
        
        
        static_assert(sizeof(typename ::bmw::common::visitor::logging_serialized_descriptor<T>::payload_type) <=
                          bmw::mw::log::detail::SharedMemoryWriter::GetMaxPayloadSize(),
                      "Serialized type too large");

        level_enabled_ = logger::instance().get_type_threshold<T>();
        default_enabled_ = level_enabled_ >= logger::instance().get_type_level<T>();

        amp::ignore = RegisterTypeGetId();
    }

    
    

    /// \public
    /// \thread-safe
    void log_serialized(const char* data, const msgsize_t size)
    
    
    {
        TrySerializeIntoSharedMemory(
            
            [&data, &size, this]()
            
            {
                logger::instance().GetSharedMemoryWriter().AllocAndWrite(
                    
                    [&data, &size](const auto data_span)
                    
                    {
                        const auto data_pointer = data_span.data();
                        std::ignore = std::copy_n(data, size, data_pointer);
                        return size;
                    },
                    shared_memory_id_,
                    size);
            });
    }

    /// \public
    /// \thread-safe
    bool enabled() const { return default_enabled_; }

    /// \public
    /// \thread-safe
    bool enabled_at(LogLevel level) const { return level_enabled_ >= level; }

    
  private:
    
    
    
    std::atomic<bmw::mw::log::detail::TypeIdentifier> shared_memory_id_;
    
    bool default_enabled_;
    LogLevel level_enabled_;
    
};

}  // namespace platform
}  // namespace bmw

/// \public
/// \thread-safe
template <typename T>
// Defined in global namespace, because it is global function, used in many files in different namespaces.
// 
inline auto& LOG_ENTRY()
{
    using DecayedType = std::decay_t<T>;
    return bmw::platform::log_entry<DecayedType>::instance();
}

/// \public
/// \thread-safe
template <typename T>
// Defined in global namespace, because it is global function, used in many files in different namespaces.
// 
inline void TRACE_LEVEL(bmw::platform::LogLevel level, const T& arg)
{
    auto& logger = LOG_ENTRY<T>();
    if (logger.enabled_at(level))
    {
        logger.TryWriteIntoSharedMemory(arg);
    }
}

/// \public
/// \thread-safe
template <typename T>
// Defined in global namespace, because it is global function, used in many files in different namespaces.
// 
inline void LOG_INTERNAL_LOGGER(const T& arg)
{
    auto& logger = LOG_ENTRY<T>();
    if (logger.enabled())
    {
        logger.TryWriteIntoSharedMemory(arg);
    }
}

/// \public
/// \thread-safe
template <typename T>
// Defined in global namespace, because it is global function, used in many files in different namespaces.
// 
inline void TRACE(const T& arg)
{
    LOG_INTERNAL_LOGGER(arg);
}

/// \public
/// \thread-safe
template <typename T>
// Defined in global namespace, because it is global function, used in many files in different namespaces.
// 
inline void TRACE_VERBOSE(const T& arg)
{
    TRACE_LEVEL(bmw::platform::LogLevel::kVerbose, arg);
}

/// \public
/// \thread-safe
template <typename T>
// Defined in global namespace, because it is global function, used in many files in different namespaces.
// 
inline void TRACE_DEBUG(const T& arg)
{
    TRACE_LEVEL(bmw::platform::LogLevel::kDebug, arg);
}

/// \public
/// \thread-safe
template <typename T>
// Defined in global namespace, because it is global function, used in many files in different namespaces.
// 
inline void TRACE_INFO(const T& arg)
{
    TRACE_LEVEL(bmw::platform::LogLevel::kInfo, arg);
}

/// \public
/// \thread-safe
template <typename T>
// Defined in global namespace, because it is global function, used in many files in different namespaces.
// 
inline void TRACE_WARNING(const T& arg)
{
    TRACE_LEVEL(bmw::platform::LogLevel::kWarn, arg);
}

/// \public
/// \thread-safe
template <typename T>
// Defined in global namespace, because it is global function, used in many files in different namespaces.
// 
inline void TRACE_ERROR(const T& arg)
{
    TRACE_LEVEL(bmw::platform::LogLevel::kError, arg);
}

/// \public
/// \thread-safe
template <typename T>
// Defined in global namespace, because it is global function, used in many files in different namespaces.
// 
inline void TRACE_FATAL(const T& arg)
{
    TRACE_LEVEL(bmw::platform::LogLevel::kFatal, arg);
}

/// \public
/// \thread-safe
template <typename T>
// Defined in global namespace, because it is global function, used in many files in different namespaces.
// 
inline void TRACE_WARN(const T& arg)
{
    TRACE_LEVEL(bmw::platform::LogLevel::kWarn, arg);
}

/// \public
/// \thread-safe
// Using preprocessor here because we define function macro to use it any where with name STRUCT_TRACEABLE
// 
#define STRUCT_TRACEABLE(...) STRUCT_VISITABLE(__VA_ARGS__)

#endif  // AAS_MW_LOG_LEGACY_NON_VERBOSE_API_TRACING_H_
