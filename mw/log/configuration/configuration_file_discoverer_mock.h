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


#ifndef PLATFORM_AAS_MW_LOG_DETAIL_CONFIG_FILE_DISCOVERER_MOCK_H
#define PLATFORM_AAS_MW_LOG_DETAIL_CONFIG_FILE_DISCOVERER_MOCK_H

#include "platform/aas/mw/log/configuration/iconfiguration_file_discoverer.h"

#include "gmock/gmock.h"

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

class ConfigurationFileDiscovererMock final : public IConfigurationFileDiscoverer
{
  public:
    
    /* Not actual for mock class; Internal GTest Framework code caused the violation; */
    
    /* Not actual for mock class; Internal GTest Framework code caused the violation; */
    MOCK_METHOD((std::vector<std::string>), FindConfigurationFiles, (), (const, noexcept, override));
    
    
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  // PLATFORM_AAS_MW_LOG_DETAIL_ICONFIG_FILE_DISCOVERER_H
