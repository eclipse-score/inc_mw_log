// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#ifndef PLATFORM_AAS_MW_LOG_NVCONFIG_H_
#define PLATFORM_AAS_MW_LOG_NVCONFIG_H_

#include "nvmsgdescriptor.h"

#include <string>
#include <unordered_map>

namespace bmw
{
namespace mw
{
namespace log
{

// 
class NvConfig
{
  public:
    enum class ReadResult : std::uint8_t
    {
        kOK = 0,
        kERROR_PARSE,
        kERROR_CONTENT
    };

    using typemap_t = std::unordered_map<std::string, config::NvMsgDescriptor>;

    explicit NvConfig(const std::string& file_path = "/bmw/platform/opt/datarouter/etc/class-id.json");

    ReadResult parseFromJson() noexcept;
    const config::NvMsgDescriptor* getDltMsgDesc(const std::string& typeName) const noexcept;

    
  private:
    
    
    const std::string json_path_;
    typemap_t typemap_;
    
};

}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  // PLATFORM_AAS_MW_LOG_NVCONFIG_H_
