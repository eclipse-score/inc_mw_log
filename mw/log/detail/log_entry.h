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


#ifndef PLATFORM_AAS_MW_LOG_DETAIL_LOG_ENTRY_H
#define PLATFORM_AAS_MW_LOG_DETAIL_LOG_ENTRY_H

#include "platform/aas/mw/log/detail/logging_identifier.h"
#include "platform/aas/mw/log/log_level.h"
#include "visitor/visit_as_struct.h"

#include <vector>

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{


using Byte = char;
using ByteVector = std::vector<Byte>;


struct LogEntry
{
    
    LoggingIdentifier app_id{""};
    LoggingIdentifier ctx_id{""};
    
    ByteVector payload{};
    std::uint64_t timestamp_steady_nsec{};
    std::uint64_t timestamp_system_nsec{};
    std::uint8_t num_of_args{};
    ByteVector header_buffer{};
    LogLevel log_level{};
#if defined __QNX__
    std::uint16_t slog2_code{0};
#endif
};

constexpr std::uint8_t GetLogLevelU8FromLogEntry(const LogEntry& entry)
{
    return static_cast<std::uint8_t>(entry.log_level);
}



/* (1) False positive: Line contains a single statement. (2) No unused stuff. (3) Expected.*/


// NOLINTBEGIN(bmw-struct-usage-compliance) justified by design
// Forward declaration for struct_visitable_impl is required for implementation
// std::forward<T>(s) added due to CB-#10171555
// 
// 
STRUCT_VISITABLE(LogEntry,
                 app_id,
                 ctx_id,
                 payload,
                 //  timestamp_steady_nsec,
                 //  timestamp_system_nsec,
                 num_of_args,
                 //  header_buffer,
                 log_level)
// NOLINTEND(bmw-struct-usage-compliance) justified by design




}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  // PLATFORM_AAS_MW_LOG_DETAIL_LOG_ENTRY_H
