// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#ifndef PLATFORM_AAS_MW_LOG_CONFIG_NVMSGDESCRIPTOR_H_
#define PLATFORM_AAS_MW_LOG_CONFIG_NVMSGDESCRIPTOR_H_

#include "platform/aas/mw/log/detail/logging_identifier.h"
#include "platform/aas/mw/log/log_level.h"

#include <string>
#include <unordered_map>


namespace bmw

{
namespace mw
{
namespace log
{
namespace config
{

// 
struct NvMsgDescriptor
{
    uint32_t id_msg_descriptor_{};
    
    bmw::mw::log::detail::LoggingIdentifier appid_{"NULL"};
    bmw::mw::log::detail::LoggingIdentifier ctxid_{"NULL"};
    
    mw::log::LogLevel logLevel_{};
};

}  // namespace config
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  // PLATFORM_AAS_MW_LOG_CONFIG_NVMSGDESCRIPTOR_H_
