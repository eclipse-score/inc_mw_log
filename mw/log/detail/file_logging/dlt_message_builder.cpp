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


#include "platform/aas/mw/log/detail/file_logging/dlt_message_builder.h"

#include "platform/aas/lib/os/utils/high_resolution_steady_clock.h"
#include "platform/aas/mw/log/detail/dlt_format.h"
#include "platform/aas/mw/log/detail/file_logging/svp_time.h"
#include "platform/aas/mw/log/log_level.h"

#include "serialization/for_logging.h"
#include "visitor/visit.h"
#include "visitor/visit_as_struct.h"

#include <arpa/inet.h>
#include <chrono>
#include <type_traits>

namespace
{

void ConstructDltStorageHeader(bmw::mw::log::detail::DltStorageHeader& storagehdr,
                               const std::uint32_t secs,
                               const std::int32_t microsecs) noexcept
{
    storagehdr.pattern[0] = std::uint8_t{'D'};
    storagehdr.pattern[1] = std::uint8_t{'L'};
    storagehdr.pattern[2] = std::uint8_t{'T'};
    storagehdr.pattern[3] = 0x01UL;
    storagehdr.seconds = secs;
    storagehdr.microseconds = microsecs;
    storagehdr.ecu[0] = std::uint8_t{'E'};
    storagehdr.ecu[1] = std::uint8_t{'C'};
    storagehdr.ecu[2] = std::uint8_t{'U'};
    storagehdr.ecu[3] = std::uint8_t{'\0'};
}

void ConstructDltStandardHeaderExtra(bmw::mw::log::detail::DltStandardHeaderExtra& standard_extra_header,
                                     const bmw::mw::log::detail::LoggingIdentifier& ecu,
                                     const std::uint32_t tmsp) noexcept
{
    static_assert(sizeof(decltype(standard_extra_header.ecu)) == sizeof(decltype(ecu.data_)),
                  "Types storing logging ID should be the same.");
    amp::ignore = std::copy(ecu.data_.begin(), ecu.data_.end(), standard_extra_header.ecu.begin());
    standard_extra_header.tmsp = htonl(tmsp);
}

void ConstructDltExtendedHeader(bmw::mw::log::detail::DltExtendedHeader& extended_header,
                                const bmw::mw::log::LogLevel log_level,
                                const std::uint8_t number_of_arguments,
                                const bmw::mw::log::detail::LoggingIdentifier& app_id,
                                const bmw::mw::log::detail::LoggingIdentifier& ctx_id) noexcept
{
    static_assert(sizeof(std::uint32_t) > sizeof(log_level), "Casting to a more capable type expected");
    const std::uint32_t level_normalized = static_cast<std::uint32_t>(log_level) & std::uint32_t{0b111UL};
    const std::uint32_t message_info = (bmw::mw::log::detail::kDltTypeLOG << bmw::mw::log::detail::kDltMsinMstpShift) |
                                       (level_normalized << bmw::mw::log::detail::kDltMsinMtinShift) |
                                       (bmw::mw::log::detail::kDltMsinVerb);
    //  static_cast operation within uint8_t range
    extended_header.msin = static_cast<std::uint8_t>(message_info);
    extended_header.noar = number_of_arguments;
    static_assert(sizeof(decltype(extended_header.apid)) == sizeof(decltype(app_id.data_)),
                  "Types storing logging ID should be the same.");
    amp::ignore = std::copy(app_id.data_.begin(), app_id.data_.end(), extended_header.apid.begin());
    static_assert(sizeof(decltype(extended_header.ctid)) == sizeof(decltype(ctx_id.data_)),
                  "Types storing logging ID should be the same.");
    amp::ignore = std::copy(ctx_id.data_.begin(), ctx_id.data_.end(), extended_header.ctid.begin());
}

template <typename T>
std::size_t GetBufferSizeCasted(T buffer_size) noexcept
{
    //  We only intend to use conversion function with human readable messages
    //  plus final memory management method will be avoiding dynamic allocation
    //  which limits maximum buffer size
    static_assert(sizeof(T) <= sizeof(std::size_t), "Buffer size conversion error");
    return static_cast<std::size_t>(buffer_size);
}

void ConstructStorageVerbosePacket(bmw::mw::log::detail::VerbosePayload& header_payload,
                                   const bmw::mw::log::detail::LogEntry& entry,
                                   const bmw::mw::log::detail::LoggingIdentifier& ecu,
                                   const std::uint8_t message_count,
                                   const bmw::mw::log::detail::SVPTime& svp_time) noexcept
{
    // truncate the message to max size if the msg size is exceeding the available buffer size
    static_assert(bmw::mw::log::detail::kDltMessageSize >
                      (bmw::mw::log::detail::kDltStorageHeaderSize + bmw::mw::log::detail::kDltHeaderSize),
                  "DLT constant values causes undefined behavior");
    const std::size_t size =
        std::min(entry.payload.size(),
                 bmw::mw::log::detail::kDltMessageSize -
                     (bmw::mw::log::detail::kDltStorageHeaderSize + bmw::mw::log::detail::kDltHeaderSize));
    static_assert(bmw::mw::log::detail::kDltMessageSize <= std::numeric_limits<std::uint16_t>::max(),
                  "Maximum size of DLT message is too big");
    //  'size' is truncated to allocate header without overflowing uint16_t value
    const auto header_size = static_cast<std::uint16_t>(bmw::mw::log::detail::kDltHeaderSize + size);

    bmw::mw::log::detail::DltStorageHeader storage_header{};
    ConstructDltStorageHeader(storage_header, svp_time.sec, svp_time.ms);

    
    amp::ignore = header_payload.Put([&storage_header](const amp::span<bmw::mw::log::detail::Byte> destination) {
        const auto destination_size = static_cast<std::size_t>(destination.size());
        const auto copy_size = std::min(destination_size, sizeof(storage_header));
        // NOLINTNEXTLINE(bmw-banned-function) memcpy is needed here
        amp::ignore = std::memcpy(destination.data(), &storage_header, copy_size);
        return copy_size;
    });
    

    bmw::mw::log::detail::DltVerboseHeader dlt_header{};
    ::bmw::mw::log::detail::ConstructDltStandardHeaderTypes(dlt_header.standard, header_size, message_count, true);
    ConstructDltStandardHeaderExtra(dlt_header.extra, ecu, svp_time.timestamp);

    ConstructDltExtendedHeader(dlt_header.extended, entry.log_level, entry.num_of_args, entry.app_id, entry.ctx_id);

    
    amp::ignore = header_payload.Put([&dlt_header](const amp::span<bmw::mw::log::detail::Byte> destination) {
        const auto copy_size = std::min(static_cast<std::size_t>(destination.size()), sizeof(dlt_header));
        // NOLINTNEXTLINE(bmw-banned-function) memcpy is needed here
        amp::ignore = std::memcpy(destination.data(), &dlt_header, copy_size);
        return copy_size;
    });
    
}

}  //  anonymous namespace

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

void ConstructDltStandardHeaderTypes(DltStandardHeader& standard,
                                     const std::uint16_t msg_size,
                                     const std::uint8_t message_count,
                                     const bool use_extended_header) noexcept
{
    //  static_cast allowed due to flags values within uint8_t range
    standard.htyp = static_cast<std::uint8_t>(kDltHtypWEID | kDltHtypWTMS | kDltHtypVERS);
    
    if (use_extended_header)
    {
        standard.htyp |= static_cast<std::uint8_t>(kDltHtypUEH);
    }
    
    standard.mcnt = message_count;
    standard.len = htons(msg_size);
}

using timestamp_t = bmw::os::HighResolutionSteadyClock::time_point;
using systime_t = std::chrono::system_clock::time_point;
using dlt_duration_t = std::chrono::duration<std::uint32_t, std::ratio<1, 10000>>;

DltMessageBuilder::DltMessageBuilder(const amp::string_view ecu_id) noexcept
    : IMessageBuilder(),
      header_payload_(kMaxDltHeaderSize, header_memory_),
      parsing_phase_{ParsingPhase::kHeader},
      ecu_id_{ecu_id},
      message_count_{0}
{
}

void DltMessageBuilder::SetNextMessage(LogRecord& log_record) noexcept
{
    log_record_ = log_record;

    const auto& entry = log_record.getLogEntry();
    const auto time_stamp = timestamp_t::clock::now().time_since_epoch();
    const auto time_epoch = systime_t::clock::now().time_since_epoch();

    using secs_u32 = std::chrono::duration<std::uint32_t, std::ratio<1>>;
    const std::uint32_t seconds = std::chrono::duration_cast<secs_u32>(time_epoch).count();
    const auto secs_remainder = time_epoch - std::chrono::seconds(seconds);

    using microsecs_i32 = std::chrono::duration<std::int32_t, std::micro>;
    const std::int32_t microsecs = std::chrono::duration_cast<microsecs_i32>(secs_remainder).count();
    const std::uint32_t timestamp = std::chrono::duration_cast<dlt_duration_t>(time_stamp).count();

    ConstructStorageVerbosePacket(header_payload_,
                                  entry,
                                  bmw::mw::log::detail::LoggingIdentifier{ecu_id_.GetStringView()},
                                  message_count_,
                                  bmw::mw::log::detail::SVPTime{timestamp, seconds, microsecs});
    message_count_++;
}

amp::optional<amp::span<const std::uint8_t>> DltMessageBuilder::GetNextSpan() noexcept
{
    if (!log_record_.has_value())
    {
        return {};
    }

    
    detail::VerbosePayload& verbose_payload = log_record_.value().get().getVerbosePayload();
    
    switch (parsing_phase_)  // LCOV_EXCL_BR_LINE: exclude the "default" branch.
    {
        case ParsingPhase::kHeader:
            parsing_phase_ = ParsingPhase::kPayload;
            return header_payload_.GetSpan();
        case ParsingPhase::kPayload:
            parsing_phase_ = ParsingPhase::kReinitialize;
            return verbose_payload.GetSpan();
        case ParsingPhase::kReinitialize:
            parsing_phase_ = ParsingPhase::kHeader;
            header_payload_.Reset();
            verbose_payload.Reset();
            log_record_.reset();
            break;
        default:  // LCOV_EXCL_LINE
            
            /* Tolerated by decision. The part of the code, which should never be reached. Excluded from code coverage
             * as well. */
            
            std::abort();  // LCOV_EXCL_LINE defensive programming: Only defined ParsingPhase values are possible to be
                           // reached.
            
            /* Tolerated by decision. The part of the code, which should never be reached. Excluded from code coverage
             * as well. */
            
            break;
    }
    return {};
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
