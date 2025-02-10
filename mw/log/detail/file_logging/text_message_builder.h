// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#ifndef PLATFORM_AAS_MW_LOG_TEXT_MESSAGE_BUILDER_H_
#define PLATFORM_AAS_MW_LOG_TEXT_MESSAGE_BUILDER_H_

#include "platform/aas/mw/log/detail/file_logging/imessage_builder.h"

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

class TextMessageBuilder : public IMessageBuilder
{
  public:
    explicit TextMessageBuilder(const amp::string_view ecu_id) noexcept;

    amp::optional<amp::span<const std::uint8_t>> GetNextSpan() noexcept override;

    void SetNextMessage(LogRecord& log_record) noexcept override;

  private:
    enum class ParsingPhase : std::uint8_t
    {
        kHeader = 0,
        kPayload,
        kReinitialize,
    };
    amp::optional<std::reference_wrapper<LogRecord>> log_record_;
    ByteVector header_memory_;
    detail::VerbosePayload header_payload_;
    ParsingPhase parsing_phase_;
    LoggingIdentifier ecu_id_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  //  PLATFORM_AAS_MW_LOG_TEXT_MESSAGE_BUILDER_H_
