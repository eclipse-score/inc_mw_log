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


#include "platform/aas/mw/log/configuration/configuration_file_discoverer.h"

#include "amp_span.hpp"
#include "amp_string_view.hpp"

#include <algorithm>

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




static const std::string kGlobalConfigPath = "/etc/ecu_logging_config.json";
static const std::string kLocalEtcConfigPath = "etc/logging.json";
static constexpr char kCwdConfigPath[] = "logging.json";
static const amp::span<const char> kEnvironmentVariableConfig = "MW_LOG_CONFIG_FILE";




}  // namespace

ConfigurationFileDiscoverer::ConfigurationFileDiscoverer(amp::pmr::unique_ptr<os::Path>&& path,
                                                         amp::pmr::unique_ptr<os::Stdlib>&& stdlib,
                                                         amp::pmr::unique_ptr<os::Unistd>&& unistd)
    : IConfigurationFileDiscoverer{}, path_(std::move(path)), stdlib_(std::move(stdlib)), unistd_(std::move(unistd))
{
}

std::vector<std::string> ConfigurationFileDiscoverer::FindConfigurationFiles() const noexcept
{
    std::vector<std::string> existing_config_files;

    const auto global_file = GetGlobalConfigFile();
    if (global_file.has_value())
    {
        existing_config_files.push_back(global_file.value());
    }

    const auto env_file = FindEnvironmentConfigFile();
    if (env_file.has_value())
    {
        existing_config_files.push_back(env_file.value());
    }
    else
    {
        const auto local_file = FindLocalConfigFile();
        if (local_file.has_value())
        {
            existing_config_files.push_back(local_file.value());
        }
    }

    return existing_config_files;
}

amp::optional<std::string> ConfigurationFileDiscoverer::GetGlobalConfigFile() const noexcept
{
    if (FileExists(kGlobalConfigPath))
    {
        return kGlobalConfigPath;
    }
    return {};
}

/// \brief Return true if the file with the given path exists.
/// Yes, a similiar utility already exists in lib/filesystem, but we cannot use it here since lib/filesystem is using
/// logging.
bool ConfigurationFileDiscoverer::FileExists(const std::string& path) const noexcept
{
    return unistd_->access(path.c_str(), bmw::os::Unistd::AccessMode::kExists).has_value();
}

amp::optional<std::string> ConfigurationFileDiscoverer::FindLocalConfigFile() const noexcept
{
    const std::vector<amp::optional<std::string>> candidates{
        {GetConfigFileByExecutableLocation(), kLocalEtcConfigPath, kCwdConfigPath}};

    const auto result = std::find_if(candidates.cbegin(), candidates.cend(), [this](const auto& candidate) {
        return (candidate.has_value() == true) && (FileExists(candidate.value()) == true);
    });

    
    if (result != candidates.cend())
    {
        return *result;
    }

    return {};
    
}


amp::optional<std::string> ConfigurationFileDiscoverer::FindEnvironmentConfigFile() const noexcept
{
    const auto environmental_config_path = stdlib_->getenv(kEnvironmentVariableConfig.data());
    if (environmental_config_path != nullptr)
    {
        if (FileExists(environmental_config_path) == true)
        {
            return environmental_config_path;
        }
    }

    return {};
}


amp::optional<std::string> ConfigurationFileDiscoverer::GetConfigFileByExecutableLocation() const noexcept
{
    const auto exec_path = path_->get_exec_path();
    if (exec_path.has_value())
    {
        const auto bin_path = path_->get_parent_dir(exec_path.value());
        const auto app_path = path_->get_parent_dir(bin_path);
        return app_path + "/" + kLocalEtcConfigPath;
    }
    return {};
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
