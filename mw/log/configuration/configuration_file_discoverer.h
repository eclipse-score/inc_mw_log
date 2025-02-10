// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#ifndef PLATFORM_AAS_MW_LOG_DETAIL_CONFIG_FILE_DISCOVERER_H
#define PLATFORM_AAS_MW_LOG_DETAIL_CONFIG_FILE_DISCOVERER_H

#include "platform/aas/mw/log/configuration/iconfiguration_file_discoverer.h"

#include <memory>

#include "amp_optional.hpp"

#include "platform/aas/lib/os/stdlib.h"
#include "platform/aas/lib/os/unistd.h"
#include "platform/aas/lib/os/utils/path.h"

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

class ConfigurationFileDiscoverer final : public IConfigurationFileDiscoverer
{
  public:
    ConfigurationFileDiscoverer(amp::pmr::unique_ptr<os::Path>&& path,
                                amp::pmr::unique_ptr<os::Stdlib>&& stdlib,
                                amp::pmr::unique_ptr<os::Unistd>&& unistd);

    std::vector<std::string> FindConfigurationFiles() const noexcept final override;

  private:
    amp::optional<std::string> GetGlobalConfigFile() const noexcept;
    bool FileExists(const std::string& path) const noexcept;
    amp::optional<std::string> FindLocalConfigFile() const noexcept;
    amp::optional<std::string> FindEnvironmentConfigFile() const noexcept;
    amp::optional<std::string> GetConfigFileByExecutableLocation() const noexcept;

    amp::pmr::unique_ptr<os::Path> path_;
    amp::pmr::unique_ptr<os::Stdlib> stdlib_;
    amp::pmr::unique_ptr<os::Unistd> unistd_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  // PLATFORM_AAS_MW_LOG_DETAIL_ICONFIG_FILE_DISCOVERER_H
