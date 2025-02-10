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
#include "platform/aas/lib/os/libgen.h"
#include "platform/aas/lib/os/mocklib/stdlib_mock.h"
#include "platform/aas/lib/os/mocklib/unistdmock.h"
#include "platform/aas/lib/os/utils/mocklib/pathmock.h"

#include "gtest/gtest.h"

using testing::_;

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

const std::string kGlobalConfigFile{"/etc/ecu_logging_config.json"};
const std::string kLocalConfigFileInPwdEtc{"etc/logging.json"};
const std::string kLocalConfigFileInPwd{"logging.json"};
const std::string kLocalConfigFileInExecPath{"/opt/app/etc/logging.json"};
const std::string kExecPath{"/opt/app/bin/app"};
const bmw::os::Error kError{bmw::os::Error::createFromErrno(0)};
static const char* kEnvConfigFilePath = "/persistent/app/logging.json";

class ConfigurationFileDiscovererFixture : public ::testing::Test
{
  public:
    void SetUp() override
    {
        ON_CALL(*unistd_mock_, access(_, _))
            .WillByDefault([this](const char* pathname, bmw::os::Unistd::AccessMode mode) {
                return this->OnAccessCall(pathname, mode);
            });

        ON_CALL(*path_mock_, get_exec_path()).WillByDefault([this]() { return this->OnExecPathCall(); });

        ON_CALL(*path_mock_, get_parent_dir(_))
            .WillByDefault([libgen_mock_ = libgen_mock_pmr_.get()](const std::string& path) {
                std::vector<std::string::value_type> path_mutable{path.begin(), path.end()};
                path_mutable.push_back('\0');
                return std::string{libgen_mock_->dirname(path_mutable.data())};
            });

        ON_CALL(*stdlib_mock_, getenv(_)).WillByDefault(::testing::Return(const_cast<char*>(kEnvConfigFilePath)));
    }

    void TearDown() override {}

    amp::expected_blank<bmw::os::Error> OnAccessCall(const char* pathname, bmw::os::Unistd::AccessMode mode)
    {
        amp::expected_blank<bmw::os::Error> result{amp::make_unexpected(kError)};

        if (mode != bmw::os::Unistd::AccessMode::kExists)
        {
            return result;
        }

        if (std::find(existing_files_.begin(), existing_files_.end(), std::string{pathname}) != existing_files_.end())
        {
            result = amp::expected_blank<bmw::os::Error>{};
        }

        return result;
    }

    amp::expected<std::string, bmw::os::Error> OnExecPathCall()
    {
        amp::expected<std::string, bmw::os::Error> result{amp::make_unexpected(kError)};

        if (!exec_path_shall_fail_)
        {
            result = kExecPath;
        }

        return result;
    }

    void AddExistingFile(std::string path) { existing_files_.push_back(path); }

    void SetExecPathShallFail(bool shall_fail = true) { exec_path_shall_fail_ = shall_fail; }

    amp::pmr::memory_resource* memory_resource_ = amp::pmr::get_default_resource();

    amp::pmr::unique_ptr<os::Libgen> libgen_mock_pmr_ = os::Libgen::Default(memory_resource_);

    amp::pmr::unique_ptr<os::UnistdMock> unistd_mock_pmr = amp::pmr::make_unique<os::UnistdMock>(memory_resource_);
    amp::pmr::unique_ptr<os::PathMock> path_mock_pmr = amp::pmr::make_unique<os::PathMock>(memory_resource_);
    amp::pmr::unique_ptr<os::StdlibMock> stdlib_mock_pmr = amp::pmr::make_unique<os::StdlibMock>(memory_resource_);

    os::UnistdMock* unistd_mock_ = unistd_mock_pmr.get();
    os::PathMock* path_mock_ = path_mock_pmr.get();
    os::StdlibMock* stdlib_mock_ = stdlib_mock_pmr.get();

    amp::pmr::unique_ptr<os::Unistd> unistd_mock_pmr_ = std::move(unistd_mock_pmr);
    amp::pmr::unique_ptr<os::Path> path_mock_pmr_ = std::move(path_mock_pmr);
    amp::pmr::unique_ptr<os::Stdlib> stdlib_mock_pmr_ = std::move(stdlib_mock_pmr);

    ConfigurationFileDiscoverer discoverer_{std::move(path_mock_pmr_),
                                            std::move(stdlib_mock_pmr_),
                                            std::move(unistd_mock_pmr_)};
    std::vector<std::string> existing_files_;
    bool exec_path_shall_fail_{};
};

TEST_F(ConfigurationFileDiscovererFixture, DiscovererShallFindGlobalConfigurationFile)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies that the discoverer shall find the global configuration file.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When ...
    AddExistingFile(kGlobalConfigFile);

    // ... then ...
    const auto result = discoverer_.FindConfigurationFiles();
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result.at(0), kGlobalConfigFile);
}

TEST_F(ConfigurationFileDiscovererFixture, DiscovererShallFindConfigurationFileInCwdEtc)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verifies that the discoverer shall find the application specific configuration file under <cwd>/etc.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When ...
    AddExistingFile(kGlobalConfigFile);
    AddExistingFile(kLocalConfigFileInPwdEtc);
    AddExistingFile(kLocalConfigFileInPwd);

    // ... then ...
    const auto result = discoverer_.FindConfigurationFiles();
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result.at(0), kGlobalConfigFile);
    EXPECT_EQ(result.at(1), kLocalConfigFileInPwdEtc);
}

TEST_F(ConfigurationFileDiscovererFixture, DiscovererShallFindConfigurationFileInCwd)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies that the discoverer shall find the application specific configuration file under <cwd>.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When ...
    AddExistingFile(kGlobalConfigFile);
    AddExistingFile(kLocalConfigFileInPwd);

    // ... then ...
    const auto result = discoverer_.FindConfigurationFiles();
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result.at(0), kGlobalConfigFile);
    EXPECT_EQ(result.at(1), kLocalConfigFileInPwd);
}

TEST_F(ConfigurationFileDiscovererFixture, DiscovererShallFindConfigurationFileInExecPath)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verifies that the discoverer shall find the application specific configuration file under the binary path.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When ...
    AddExistingFile(kGlobalConfigFile);
    AddExistingFile(kLocalConfigFileInExecPath);
    AddExistingFile(kLocalConfigFileInPwdEtc);
    AddExistingFile(kLocalConfigFileInPwd);

    // ... then ...
    const auto result = discoverer_.FindConfigurationFiles();
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result.at(0), kGlobalConfigFile);
    EXPECT_EQ(result.at(1), kLocalConfigFileInExecPath);
}

TEST_F(ConfigurationFileDiscovererFixture, DiscovererShallFindConfigurationFileInEnvPath)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies that the discoverer shall find the application specific configuration file under the "
                   "environment variable path.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When ...
    AddExistingFile(kGlobalConfigFile);
    AddExistingFile(kEnvConfigFilePath);

    // ... then ...
    const auto result = discoverer_.FindConfigurationFiles();
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result.at(0), kGlobalConfigFile);
    EXPECT_EQ(result.at(1), kEnvConfigFilePath);
}

TEST_F(ConfigurationFileDiscovererFixture, DiscovererShallFindConfigurationFileInEnvPathOverrideCwdEtc)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies that the discoverer shall find the application specific configuration file under the "
                   "environment variable path. The <cwd>/etc. path should be ignored");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When ...
    AddExistingFile(kGlobalConfigFile);
    AddExistingFile(kLocalConfigFileInPwdEtc);
    AddExistingFile(kEnvConfigFilePath);

    // ... then ...
    const auto result = discoverer_.FindConfigurationFiles();
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result.at(0), kGlobalConfigFile);
    EXPECT_EQ(result.at(1), kEnvConfigFilePath);
}

TEST_F(ConfigurationFileDiscovererFixture, DiscovererShallFindConfigurationFileInEnvPathOverrideCwd)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies that the discoverer shall find the application specific configuration file under the "
                   "environment variable path. The <cwd> path should be ignored");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When ...
    AddExistingFile(kGlobalConfigFile);
    AddExistingFile(kLocalConfigFileInPwd);
    AddExistingFile(kEnvConfigFilePath);

    // ... then ...
    const auto result = discoverer_.FindConfigurationFiles();
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result.at(0), kGlobalConfigFile);
    EXPECT_EQ(result.at(1), kEnvConfigFilePath);
}

TEST_F(ConfigurationFileDiscovererFixture, DiscovererShallFindConfigurationFileInEnvPathOverrideExecPath)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies that the discoverer shall find the application specific configuration file under the "
                   "environment variable path. The binary path should be ignored");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When ...
    AddExistingFile(kGlobalConfigFile);
    AddExistingFile(kLocalConfigFileInExecPath);
    AddExistingFile(kEnvConfigFilePath);

    // ... then ...
    const auto result = discoverer_.FindConfigurationFiles();
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result.at(0), kGlobalConfigFile);
    EXPECT_EQ(result.at(1), kEnvConfigFilePath);
}

TEST_F(ConfigurationFileDiscovererFixture, DiscovererShallReturnEmptyIfNothingExists)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies that the discoverer shall return an empty list if no config file exists.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When no file exists then ...
    const auto result = discoverer_.FindConfigurationFiles();
    EXPECT_EQ(result.size(), 0);
}

TEST_F(ConfigurationFileDiscovererFixture, DiscovererShallReturnEmptyIfExecPathFails)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies that the discoverer shall return an empty list if an error occurs.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When...
    AddExistingFile(kLocalConfigFileInExecPath);
    SetExecPathShallFail();

    // ... then ...
    const auto result = discoverer_.FindConfigurationFiles();
    EXPECT_EQ(result.size(), 0);
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
