// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#ifndef PLATFORM_AAS_MW_LOG_DETAIL_TARGET_CONFIG_READER_H
#define PLATFORM_AAS_MW_LOG_DETAIL_TARGET_CONFIG_READER_H

#include "platform/aas/lib/os/unistd.h"
#include "platform/aas/lib/os/utils/path_impl.h"
#include "platform/aas/mw/log/configuration/configuration_file_discoverer.h"
#include "platform/aas/mw/log/configuration/itarget_config_reader.h"

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

class TargetConfigReader final : public ITargetConfigReader
{
  public:
    explicit TargetConfigReader(std::unique_ptr<IConfigurationFileDiscoverer> discoverer) noexcept;

    bmw::Result<Configuration> ReadConfig() const noexcept final override;

  private:
    std::unique_ptr<IConfigurationFileDiscoverer> discoverer_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  // PLATFORM_AAS_MW_LOG_DETAIL_TARGET_CONFIG_READER_H
