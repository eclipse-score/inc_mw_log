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


#include "platform/aas/mw/log/detail/recorder_factory.h"

#include "platform/aas/mw/log/configuration/target_config_reader_mock.h"
#include "platform/aas/mw/log/detail/composite_recorder.h"
#include "platform/aas/mw/log/detail/data_router/data_router_recorder.h"
#include "platform/aas/mw/log/detail/empty_recorder.h"
#include "platform/aas/mw/log/detail/error.h"
#include "platform/aas/mw/log/detail/file_logging/file_recorder.h"
#include "platform/aas/mw/log/detail/file_logging/text_recorder.h"

#include "platform/aas/lib/os/mocklib/fcntl_mock.h"

#include "gtest/gtest.h"

#include <type_traits>

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

using ::testing::_;
using ::testing::Return;

template <typename ConcreteRecorder>
bool IsRecorderOfType(const std::unique_ptr<Recorder>& recorder) noexcept
{
    static_assert(std::is_base_of<Recorder, ConcreteRecorder>::value,
                  "Concrete recorder shall be derived from Recorder base class");

    return dynamic_cast<const ConcreteRecorder*>(recorder.get()) != nullptr;
}

template <typename ConcreteRecorder>
bool ContainsRecorderOfType(const CompositeRecorder& composite) noexcept
{
    bool return_value = false;
    for (const auto& recorder : composite.GetRecorders())
    {
        if (IsRecorderOfType<ConcreteRecorder>(recorder))
        {
            return_value = true;
            break;
        }
    }
    return return_value;
}

class RecorderFactoryConfigFixture : public ::testing::Test
{
  public:
    void SetUp() override { memory_resource_ = amp::pmr::get_default_resource(); }
    void TearDown() override {}

    std::unique_ptr<Recorder> CreateFromConfiguration() noexcept
    {
        auto config_reader_mock = std::make_unique<TargetConfigReaderMock>();
        ON_CALL(*config_reader_mock, ReadConfig).WillByDefault(testing::Invoke([&]() { return config_result_; }));
        return RecorderFactory{}.CreateFromConfiguration(std::move(config_reader_mock),
                                                         amp::pmr::get_default_resource());
    }

    void SetTargetConfigReaderResult(bmw::Result<Configuration> result) noexcept { config_result_ = result; }

    void SetConfigurationWithLogMode(const std::unordered_set<LogMode>& log_modes,
                                     Configuration config = Configuration{}) noexcept
    {
        config.SetLogMode(log_modes);
        SetTargetConfigReaderResult(config);
    }

  protected:
    bmw::Result<Configuration> config_result_;
    amp::pmr::memory_resource* memory_resource_ = nullptr;
};

TEST_F(RecorderFactoryConfigFixture, ConfigurationMemoryErrorShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory will return empty recorder in case of configuration error.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    amp::pmr::memory_resource* null_memory_resource = nullptr;
    auto recorder = RecorderFactory{}.CreateFromConfiguration(null_memory_resource);
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, ConfigurationWithConfigReaderMemoryErrorShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory will return empty recorder in case of configuration error.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto config_reader_mock = std::make_unique<TargetConfigReaderMock>();
    amp::pmr::memory_resource* null_memory_resource = nullptr;
    auto recorder = RecorderFactory{}.CreateFromConfiguration(std::move(config_reader_mock), null_memory_resource);
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, CreateConsoleLoggingOnlyMemoryErrorShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Create Console Logging Recorder will return empty recorder in case of memory error.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    amp::pmr::memory_resource* null_memory_resource = nullptr;
    auto recorder = RecorderFactory{}.CreateWithConsoleLoggingOnly(null_memory_resource);
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, CreateRecorderFromLogModeMemoryErrorShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Recorder created using LogMode will return empty recorder in case of memory error.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    amp::pmr::memory_resource* null_memory_resource = nullptr;
    Configuration config{};
    auto fcntl_mock = amp::pmr::make_unique<bmw::os::FcntlMock>(memory_resource_);
    auto recorder = RecorderFactory{}.CreateRecorderFromLogMode(
        LogMode::kFile, config, std::move(fcntl_mock), null_memory_resource);
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, ConfigurationErrorShallFallbackToConsoleLogging)
{
    RecordProperty("Requirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory will return text recorder in case of configuration error.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    SetTargetConfigReaderResult(bmw::MakeUnexpected(Error::kConfigurationFilesNotFound));
    auto recorder = CreateFromConfiguration();
    EXPECT_TRUE(IsRecorderOfType<TextRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, NoRecorderConfiguredShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory can return empty recorder if no recorder is configured.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // TargetConfigReader shall return a config with no active recorders.
    SetConfigurationWithLogMode({});

    auto recorder = CreateFromConfiguration();
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, RemoteConfiguredShallReturnDataRouterRecorder)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory can create DataRouterRecorder if remote is configured.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    SetConfigurationWithLogMode({LogMode::kRemote});
    auto recorder = CreateFromConfiguration();
    EXPECT_TRUE(IsRecorderOfType<DataRouterRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, FileConfiguredShallReturnFileRecorder)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory can create FileRecorder if file is configured.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    SetConfigurationWithLogMode({LogMode::kFile});
    auto recorder = CreateFromConfiguration();
    EXPECT_TRUE(IsRecorderOfType<FileRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, FileConfiguredShallReturnEmptyRecorderWithInvalidFile)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory can create FileRecorder if file is configured.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    Configuration config;
    constexpr auto kInvalidPath = "!@#";
    config.SetLogFilePath(kInvalidPath);
    SetConfigurationWithLogMode({LogMode::kFile}, config);
    auto recorder = CreateFromConfiguration();
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, FileConfigurionShallCallFileCreationAndReturnFileRecorder)
{
    RecordProperty("Requirement", ", 7");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "RecorderFactory can create FileRecorder and fileCreation will be called if file is configured. The "
                   "component shall set the FD_CLOEXEC (or O_CLOEXEC) flag on all the file descriptor it owns");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto fcntl_mock = amp::pmr::make_unique<bmw::os::FcntlMock>(memory_resource_);
    auto fcntl_mock_raw_ptr = fcntl_mock.get();
    std::int32_t file_descriptor = 3;

    const bmw::os::Fcntl::Open open_flags =
        bmw::os::Fcntl::Open::kWriteOnly | bmw::os::Fcntl::Open::kCreate | bmw::os::Fcntl::Open::kCloseOnExec;
    const bmw::os::Stat::Mode access_flags = bmw::os::Stat::Mode::kReadUser | bmw::os::Stat::Mode::kWriteUser |
                                             bmw::os::Stat::Mode::kReadGroup | bmw::os::Stat::Mode::kReadOthers;

    EXPECT_CALL(*fcntl_mock_raw_ptr, open(_, open_flags, access_flags)).Times(1).WillOnce(Return(file_descriptor));

    Configuration config;

    auto recorder =
        RecorderFactory{}.CreateRecorderFromLogMode(LogMode::kFile, config, std::move(fcntl_mock), memory_resource_);

    EXPECT_TRUE(IsRecorderOfType<FileRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, ConsoleConfiguredShallReturnConsoleRecorder)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory can create ConsoleRecorder if console is configured.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    SetConfigurationWithLogMode({LogMode::kConsole});
    auto recorder = CreateFromConfiguration();
    EXPECT_TRUE(IsRecorderOfType<TextRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, InvalidLogModeShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory will create EmptyRecorder in case of invalid log mode.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    SetConfigurationWithLogMode({LogMode::kInvalid});
    auto recorder = CreateFromConfiguration();
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, MultipleLogModesShallReturnCompositeRecorder)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "RecorderFactory shall create CompositeRecorder in case of multiple log mode is configured.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    SetConfigurationWithLogMode({LogMode::kFile, LogMode::kConsole, LogMode::kRemote});
    auto recorder = CreateFromConfiguration();
    ASSERT_TRUE(IsRecorderOfType<CompositeRecorder>(recorder));
    const auto& composite_recorder = *dynamic_cast<CompositeRecorder*>(recorder.get());

    EXPECT_EQ(composite_recorder.GetRecorders().size(), config_result_->GetLogMode().size());
    EXPECT_TRUE(ContainsRecorderOfType<DataRouterRecorder>(composite_recorder));
    EXPECT_TRUE(ContainsRecorderOfType<FileRecorder>(composite_recorder));
    EXPECT_TRUE(ContainsRecorderOfType<TextRecorder>(composite_recorder));
}

TEST_F(RecorderFactoryConfigFixture, CreateWithConsoleLoggingOnlyShallReturnConsoleRecorder)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of creating a specific logging.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto recorder = RecorderFactory{}.CreateWithConsoleLoggingOnly(amp::pmr::get_default_resource());

    EXPECT_TRUE(IsRecorderOfType<TextRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, CreateStubShallReturnEmptyRecorder)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "RecorderFactory shall create EmptyRecorder in case of using CreateStub.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto recorder = RecorderFactory{}.CreateStub();

    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

TEST_F(RecorderFactoryConfigFixture, SystemConfiguredShallReturnSlogRecorder)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("Description",
                   "The system logger backend shall be enabled if and only if the log mode contains 'kSystem'");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    SetConfigurationWithLogMode({LogMode::kSystem});
    auto recorder = CreateFromConfiguration();

#if defined(__QNXNTO__)
    // Console recorder shall be reused for slog backend. For slogger we also need to output ASCII like on the console.
    EXPECT_TRUE(IsRecorderOfType<TextRecorder>(recorder));
#else
    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
#endif  //__QNXNTO__
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
