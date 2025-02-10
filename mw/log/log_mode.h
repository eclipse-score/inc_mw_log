// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#ifndef PLATFORM_AAS_MW_LOG_LOG_MODE_H
#define PLATFORM_AAS_MW_LOG_LOG_MODE_H

#include <cstdint>

namespace bmw
{
namespace mw
{

///
/// \brief Log mode. Flags, used to configure the sink for log messages.
/// \public
/// \details Flags can be combined.
///
/// \Requirement{SWS_LOG_00019}
////
enum class LogMode : uint8_t
{
    kRemote = 0x01,   ///< Sent remotely
    kFile = 0x02,     ///< Save to file
    kConsole = 0x04,  ///< Forward to console,
    kSystem = 0x08,   ///< QNX: forward to slog,
    kInvalid = 0xff   ///< Invalid log mode,
};

}  // namespace mw
}  // namespace bmw

#endif  // PLATFORM_AAS_MW_LOG_LOG_LEVEL_H
