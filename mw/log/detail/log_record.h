// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************




#ifndef PLATFORM_AAS_MW_LOG_DETAIL_RECORD_H
#define PLATFORM_AAS_MW_LOG_DETAIL_RECORD_H

#include "platform/aas/mw/log/detail/log_entry.h"
#include "platform/aas/mw/log/detail/verbose_payload.h"

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{


class LogRecord
{
  public:
    explicit LogRecord(const std::size_t max_payload_size_bytes = 255U) noexcept;

    LogEntry& getLogEntry() noexcept;
    const LogEntry& getLogEntry() const noexcept;
    detail::VerbosePayload& getVerbosePayload() noexcept;
    const detail::VerbosePayload& getVerbosePayload() const noexcept;

    /// Rule of five: Need manual constructors and assignment operator to update the internal reference correctly.
    ~LogRecord() noexcept = default;
    LogRecord(const LogRecord&) noexcept;
    LogRecord(LogRecord&&) noexcept;
    LogRecord& operator=(const LogRecord&) noexcept;
    LogRecord& operator=(LogRecord&&) noexcept;

  private:
    LogEntry logEntry_;

    // Caution: contains a reference to logEntry_ internals.
    detail::VerbosePayload verbosePayload_;
};


}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif
