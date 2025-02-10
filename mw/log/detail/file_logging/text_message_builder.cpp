// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************




#include "platform/aas/mw/log/detail/file_logging/text_message_builder.h"

#include "platform/aas/lib/os/utils/high_resolution_steady_clock.h"
#include "platform/aas/mw/log/detail/file_logging/text_format.h"

#include <chrono>

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


constexpr std::size_t kMaxHeaderSize = std::size_t{512};



void LogLevelToString(VerbosePayload& payload, const LogLevel level) noexcept

{
    switch (level)
    {
        case LogLevel::kOff:
            detail::TextFormat::Log(payload, amp::string_view{"off"});
            break;
        case LogLevel::kFatal:
            detail::TextFormat::Log(payload, amp::string_view{"fatal"});
            break;
        case LogLevel::kError:
            detail::TextFormat::Log(payload, amp::string_view{"error"});
            break;
        case LogLevel::kWarn:
            detail::TextFormat::Log(payload, amp::string_view{"warn"});
            break;
        case LogLevel::kInfo:
            detail::TextFormat::Log(payload, amp::string_view{"info"});
            break;
        case LogLevel::kDebug:
            detail::TextFormat::Log(payload, amp::string_view{"debug"});
            break;
        case LogLevel::kVerbose:
            detail::TextFormat::Log(payload, amp::string_view{"verbose"});
            break;
        default:
            detail::TextFormat::Log(payload, amp::string_view{"undefined"});
            break;
    }
}
inline std::uint32_t TimeStamp() noexcept
{
    const std::uint32_t timestamp = std::chrono::duration_cast<std::chrono::duration<uint32_t, std::ratio<1, 10000>>>(
                                        bmw::os::HighResolutionSteadyClock::now().time_since_epoch())
                                        .count();
    return timestamp;
}

}  // namespace


TextMessageBuilder::TextMessageBuilder(const amp::string_view ecu_id) noexcept
    : IMessageBuilder(),
      header_payload_(kMaxHeaderSize, header_memory_),
      parsing_phase_{ParsingPhase::kHeader},
      ecu_id_{ecu_id}
{
}


void TextMessageBuilder::SetNextMessage(LogRecord& log_record) noexcept

{
    log_record_ = log_record;

    const auto& log_entry = log_record_.value().get().getLogEntry();
    detail::TextFormat::PutFormattedTime(header_payload_);
    detail::TextFormat::Log(header_payload_, TimeStamp());
    detail::TextFormat::Log(header_payload_, amp::string_view{"000"});
    detail::TextFormat::Log(header_payload_, ecu_id_.GetStringView());
    detail::TextFormat::Log(header_payload_, log_entry.app_id.GetStringView());
    detail::TextFormat::Log(header_payload_, log_entry.ctx_id.GetStringView());
    detail::TextFormat::Log(header_payload_, amp::string_view{"log"});
    LogLevelToString(header_payload_, log_entry.log_level);
    detail::TextFormat::Log(header_payload_, amp::string_view{"verbose"});
    detail::TextFormat::Log(header_payload_, log_entry.num_of_args);
    parsing_phase_ = ParsingPhase::kHeader;
}



amp::optional<amp::span<const std::uint8_t>> TextMessageBuilder::GetNextSpan() noexcept


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
            break;
        case ParsingPhase::kPayload:
            parsing_phase_ = ParsingPhase::kReinitialize;
            detail::TextFormat::TerminateLog(verbose_payload);
            return log_record_.value().get().getVerbosePayload().GetSpan();
            break;
        case ParsingPhase::kReinitialize:
            parsing_phase_ = ParsingPhase::kHeader;
            header_payload_.Reset();
            verbose_payload.Reset();
            log_record_.reset();
            break;
        default:  // LCOV_EXCL_LINE
            
            /* Tolerated by decision. The part of the code, which should never be reached. Excluded from code coverage
             * as well. */
            
            /* Tolerated by decision. The part of the code, which should never be reached. Excluded from code coverage
             * as well. */
            std::abort();  // LCOV_EXCL_LINE defensive programming: Only defined ParsingPhase values are possible to be
                           // reached.
            
            /* Tolerated by decision. The part of the code, which should never be reached. Excluded from code coverage
             * as well. */
            
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
