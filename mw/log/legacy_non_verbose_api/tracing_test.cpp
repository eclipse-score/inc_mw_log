// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#include "platform/aas/mw/log/detail/data_router/data_router_backend.h"

#include "platform/aas/lib/os/mocklib/stat_mock.h"
#include "platform/aas/lib/os/mocklib/stdlib_mock.h"
#include "platform/aas/mw/log/detail/data_router/shared_memory/shared_memory_reader.h"
#include "platform/aas/mw/log/legacy_non_verbose_api/tracing.h"

#include "platform/aas/pas/logging/include/filetransfer/filetransfer_message.h"
#include "platform/aas/pas/logging/include/filetransfer/filetransfer_message_trace.h"

#include "serialization/for_logging.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <optional>

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
using ::testing::AnyNumber;
using ::testing::Return;

using SerializeNs = ::bmw::common::visitor::logging_serializer;
const std::string ERROR_CONTENT_1_PATH =
    "platform/aas/mw/log/legacy_non_verbose_api/test/error-content-json-class-id.json";
const std::string JSON_PATH = "platform/aas/mw/log/legacy_non_verbose_api/test/test-class-id.json";
struct DatarouterMessageClientStub : DatarouterMessageClient
{
    void Run() override {}
    void Shutdown() override {}
};

class DatarouterMessageClientStubFactory : public DatarouterMessageClientFactory
{
  public:
    std::unique_ptr<DatarouterMessageClient> CreateOnce(const std::string&, const std::string&) override
    {
        return std::make_unique<DatarouterMessageClientStub>();
    }
};

class LoggerFixture : public ::testing::Test
{
  public:
    void PrepareFixture(bmw::mw::log::NvConfig nv_config, uint64_t size = 1024UL)
    {
        auto kBufferSize = size;
        buffer1_.resize(kBufferSize);
        buffer2_.resize(kBufferSize);
        shared_data_.control_block.control_block_even.data = {buffer1_.data(), static_cast<int64_t>(kBufferSize)};
        shared_data_.control_block.control_block_odd.data = {buffer2_.data(), static_cast<int64_t>(kBufferSize)};
        shared_data_.control_block.switch_count_points_active_for_writing = std::uint32_t{1};

        AlternatingReadOnlyReader read_only_reader{shared_data_.control_block,
                                                   shared_data_.control_block.control_block_even.data,
                                                   shared_data_.control_block.control_block_odd.data};
        reader_ = std::make_unique<SharedMemoryReader>(shared_data_, std::move(read_only_reader), []() noexcept {});

        SharedMemoryWriter writer{shared_data_, []() noexcept {}};
        const amp::string_view kCtx{"STDA"};
        const ContextLogLevelMap context_log_level_map{{LoggingIdentifier{kCtx}, LogLevel::kError}};
        config_.SetContextLogLevel(context_log_level_map);
        logger_ = std::make_unique<bmw::platform::logger>(config_, nv_config, std::move(writer));
        ::bmw::platform::logger::InjectTestInstance(logger_.get());
    }

    void PrepareContextLogLevelFixture(bmw::mw::log::NvConfig nv_config, const amp::string_view ctxid)
    {
        SharedMemoryWriter writer{shared_data_, []() noexcept {}};
        const ContextLogLevelMap context_log_level_map{{LoggingIdentifier{ctxid}, LogLevel::kError}};
        config_.SetContextLogLevel(context_log_level_map);
        logger_ = std::make_unique<bmw::platform::logger>(config_, nv_config, std::move(writer));
        ::bmw::platform::logger::InjectTestInstance(logger_.get());
    }

    void TearDown() override { ::bmw::platform::logger::InjectTestInstance(nullptr); }
    void SimulateLogging(const LogLevel logLevel = LogLevel::kError,
                         const std::string& context_id = "xxxx",
                         const std::string& app_id = "xxxx")
    {

        const auto slot = unit_.ReserveSlot().value();

        auto&& logRecord = unit_.GetLogRecord(slot);
        auto& log_entry = logRecord.getLogEntry();

        log_entry.app_id = LoggingIdentifier{app_id};
        log_entry.ctx_id = LoggingIdentifier{context_id};
        log_entry.log_level = logLevel;
        log_entry.num_of_args = 5;
        logRecord.getVerbosePayload().Put("xyz xyz", 7);

        unit_.FlushSlot(slot);

        const auto acquire_result = logger_->GetSharedMemoryWriter().ReadAcquire();
        config_ = logger_->get_config();

        reader_->NotifyAcquisitionSetReader(acquire_result);

        reader_->Read([](const TypeRegistration&) noexcept {},
                      [this](const SharedMemoryRecord& record) {
                          amp::ignore = SerializeNs::deserialize(
                              record.payload.data(), GetDataSizeAsLength(record.payload), header_);
                      });
    }

    Configuration config_{};
    std::unique_ptr<bmw::platform::logger> logger_{};
    LogEntry header_{};

  private:
    SharedData shared_data_{};
    std::unique_ptr<SharedMemoryReader> reader_{};
    DatarouterMessageClientStubFactory message_client_factory_{};

    std::vector<Byte> buffer1_{};
    std::vector<Byte> buffer2_{};
    DataRouterBackend unit_{std::uint8_t{255UL}, LogRecord{}, message_client_factory_, config_, WriterFactory{{}}};
};

TEST_F(LoggerFixture, WhenCreatingSharedMemoryWriterwithNotEnoughBufferSizeRegesteringNewTypeShallFail)
{
    RecordProperty("Requirement", ",,");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "When Creating Shared memory writer with not enough buffer size, registering new type shall fail");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PrepareFixture(bmw::mw::log::NvConfig{JSON_PATH}, 1);
    SimulateLogging();
}

TEST_F(LoggerFixture, WhenCreatingSharedMemoryWriterwithOneKiloBytesBufferSizeRegesteringNewTypeShallFail)
{
    RecordProperty("Requirement", ",,");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "When Creating Shared memory writer with not enough buffer size (1 kiloBytes), registering new type "
                   "shall fail");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    constexpr auto kBufferSize = 1024UL;
    PrepareFixture(bmw::mw::log::NvConfig{JSON_PATH}, kBufferSize);
    SimulateLogging();
}

TEST_F(LoggerFixture, WhenProvidingCorrectNvConfigGetTypeLevelAndThreshold)
{
    RecordProperty("Requirement", ",");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "The log message shall be disabled if the log level is above to the threshold.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PrepareFixture(bmw::mw::log::NvConfig{JSON_PATH});
    EXPECT_EQ(bmw::platform::LogLevel::kError, logger_->get_type_level<bmw::mw::log::detail::LogEntry>());
    EXPECT_EQ(bmw::platform::LogLevel::kError, logger_->get_type_threshold<bmw::mw::log::detail::LogEntry>());
}

TEST_F(LoggerFixture, WhenProvidingNvConfigWithErrorShallGetErrorContent)
{
    RecordProperty("Requirement", ",,");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Unable to parse the JSON file due to error in the content.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PrepareFixture(bmw::mw::log::NvConfig{ERROR_CONTENT_1_PATH});
    bmw::mw::log::NvConfig nv = logger_->get_non_verbose_config();
    EXPECT_EQ(bmw::mw::log::NvConfig::ReadResult::kERROR_CONTENT, nv.parseFromJson());
}

TEST(LoggerFallback, WhenProperWriterNotProvidedFailSafeFallbackShallBeReturned)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verify the ability of returning failsafe fallback in case of a wrong writer was provided.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    amp::optional<SharedMemoryWriter> writer{amp::nullopt};
    Configuration config_{};
    bmw::mw::log::NvConfig nv_config;
    std::unique_ptr<bmw::platform::logger> logger{};
    logger = std::make_unique<bmw::platform::logger>(config_, nv_config, std::move(writer));

    const auto acquire_result = logger->GetSharedMemoryWriter().ReadAcquire();
    EXPECT_EQ(acquire_result.acquired_buffer, 1UL);
}

TEST(LoggerFallback, AllArgsNulloptShallReturnFailsafeFallback)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verify the ability of returning failsafe fallback in case of initialize logger with no arguments.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::unique_ptr<bmw::platform::logger> logger{};
    logger = std::make_unique<bmw::platform::logger>(amp::nullopt, amp::nullopt, amp::nullopt);

    const auto acquire_result = logger->GetSharedMemoryWriter().ReadAcquire();
    EXPECT_EQ(acquire_result.acquired_buffer, 1UL);

    constexpr Length kSmallRequest{1UL};

    //  AllocAndWrite shall discard operation by providing empty span:
    logger->GetSharedMemoryWriter().AllocAndWrite(
        [](const auto data_span) noexcept {
            EXPECT_EQ(data_span.size(), 0);
            return 0UL;
        },
        TypeIdentifier{1UL},
        kSmallRequest);
}

TEST_F(LoggerFixture, WhenProvidingWrongCtxIdWillLeadToVerboseLogLevelThreshold)
{
    RecordProperty("Requirement", ",");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "The log level should set to verbose when providing wrong ctx id.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    PrepareContextLogLevelFixture(bmw::mw::log::NvConfig{JSON_PATH}, "not supported ctx id");
    EXPECT_EQ(bmw::platform::LogLevel::kVerbose, logger_->get_type_threshold<bmw::mw::log::detail::LogEntry>());
}

TEST_F(LoggerFixture, GetSharedMemoryWriterShallFailWhenThereIsNoSharedMemoryAllocatedUsingLoggerInstanceInitialization)
{
    RecordProperty("Requirement", ",");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "The code should terminate when getting the shared memory writer when there is no shared memory allocated. ");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    amp::optional<bmw::mw::log::detail::SharedMemoryWriter> shared_memory{};
    logger_ =
        std::make_unique<bmw::platform::logger>(config_, bmw::mw::log::NvConfig{JSON_PATH}, std::move(shared_memory));

    const auto acquire_result = logger_->GetSharedMemoryWriter().ReadAcquire();
    EXPECT_EQ(acquire_result.acquired_buffer, 1UL);
}

TEST_F(
    LoggerFixture,
    GetSharedMemoryWriterShallFailWhenThereIsNoSharedMemoryAllocatedUsingLoggerInstanceInitializationAndCallingRegisterType)
{
    RecordProperty("Requirement", ",");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "The code should terminate when getting the shared-memory writer if there is no shared memory "
                   "allocated during the RegisterType call.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    amp::optional<bmw::mw::log::detail::SharedMemoryWriter> shared_memory;
    logger_ =
        std::make_unique<bmw::platform::logger>(config_, bmw::mw::log::NvConfig{JSON_PATH}, std::move(shared_memory));
    logger_->RegisterType<bmw::logging::FileTransferEntry>();

    const auto acquire_result = logger_->GetSharedMemoryWriter().ReadAcquire();
    EXPECT_EQ(acquire_result.acquired_buffer, 1UL);
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
