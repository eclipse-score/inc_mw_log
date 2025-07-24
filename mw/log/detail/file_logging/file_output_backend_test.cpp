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


#include "platform/aas/mw/log/detail/file_logging/file_output_backend.h"

#include "platform/aas/mw/log/configuration/configuration.h"
#include "platform/aas/mw/log/detail/circular_allocator.h"
#include "platform/aas/mw/log/detail/error.h"
#include "platform/aas/mw/log/detail/file_logging/mock/message_builder_mock.h"

#include "platform/aas/lib/os/mocklib/fcntl_mock.h"
#include "platform/aas/lib/os/mocklib/unistdmock.h"

#include "gtest/gtest.h"

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

using SpanData = amp::span<const std::uint8_t>;
using OptionalSpan = amp::optional<SpanData>;
using ::testing::_;
using ::testing::Exactly;
using ::testing::Return;

class FileOutputBackendFixture : public ::testing::Test
{
  public:
    void SetUp() override
    {
        allocator_ = std::make_unique<CircularAllocator<LogRecord>>(pool_size_);
        raw_allocator_ptr_ = allocator_.get();

        message_builder_mock_ = std::make_unique<mock::MessageBuilderMock>();
        raw_message_builder_mock_ = message_builder_mock_.get();
        message_builder = std::move(message_builder_mock_);
        memory_resource_ = amp::pmr::get_default_resource();
    }
    void TearDown() override {}

  private:
  protected:
    const std::size_t pool_size_ = 4;
    const std::uint8_t data_table_[4] = {};
    std::unique_ptr<CircularAllocator<LogRecord>> allocator_ = nullptr;
    CircularAllocator<LogRecord>* raw_allocator_ptr_ = nullptr;
    std::unique_ptr<IMessageBuilder> message_builder = nullptr;
    std::unique_ptr<mock::MessageBuilderMock> message_builder_mock_ = nullptr;
    mock::MessageBuilderMock* raw_message_builder_mock_ = nullptr;
    std::int32_t file_descriptor_ = 23;
    amp::pmr::memory_resource* memory_resource_ = nullptr;
};

TEST_F(FileOutputBackendFixture, ReserveSlotShouldTriggerFlushing)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "ReserveSlot shall trigger flushing.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto fcntl_mock = amp::pmr::make_unique<bmw::os::FcntlMock>(memory_resource_);
    auto unistd_mock = amp::pmr::make_unique<bmw::os::UnistdMock>(memory_resource_);
    FileOutputBackend unit(std::move(message_builder),
                           file_descriptor_,
                           std::move(allocator_),
                           std::move(fcntl_mock),
                           std::move(unistd_mock));

    EXPECT_CALL(*raw_message_builder_mock_, GetNextSpan)
        .WillOnce(Return(OptionalSpan{}))
        .WillOnce(Return(OptionalSpan{}));

    auto slot = unit.ReserveSlot();
    EXPECT_TRUE(slot.has_value());
}

TEST_F(FileOutputBackendFixture, FlushSlotShouldTriggerFlushing)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FlushSlot shall trigger flushing.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto fcntl_mock = amp::pmr::make_unique<bmw::os::FcntlMock>(memory_resource_);
    auto unistd_mock = amp::pmr::make_unique<bmw::os::UnistdMock>(memory_resource_);

    const auto& slot_index = allocator_->AcquireSlotToWrite();
    FileOutputBackend unit(std::move(message_builder),
                           file_descriptor_,
                           std::move(allocator_),
                           std::move(fcntl_mock),
                           std::move(unistd_mock));

    EXPECT_CALL(*raw_message_builder_mock_, GetNextSpan)
        .WillOnce(Return(OptionalSpan{}))                              //  first unitialized
        .WillOnce(Return(SpanData(data_table_, sizeof(data_table_))))  //  actual data to be written
        .WillRepeatedly(Return(OptionalSpan{}));

    EXPECT_CALL(*raw_message_builder_mock_, SetNextMessage(_)).Times(Exactly(1));

    unit.FlushSlot(SlotHandle{static_cast<SlotIndex>(slot_index.value())});
}

TEST_F(FileOutputBackendFixture, DepletedAllocatorShouldCauseEmptyOptionalReturn)
{
    RecordProperty("Requirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "ReserveSlot will return None if all allocator's slots are reserved.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    for (std::size_t i = 0; i < pool_size_; i++)
    {
        const auto& slot = allocator_->AcquireSlotToWrite();
        EXPECT_TRUE(slot.has_value());
    }

    auto fcntl_mock = amp::pmr::make_unique<bmw::os::FcntlMock>(memory_resource_);
    auto unistd_mock = amp::pmr::make_unique<bmw::os::UnistdMock>(memory_resource_);

    FileOutputBackend unit(std::move(message_builder),
                           file_descriptor_,
                           std::move(allocator_),
                           std::move(fcntl_mock),
                           std::move(unistd_mock));

    EXPECT_CALL(*raw_message_builder_mock_, GetNextSpan)
        .WillOnce(Return(OptionalSpan{}))  //  first unitialized
        .WillRepeatedly(Return(OptionalSpan{}));

    auto slot = unit.ReserveSlot();
    EXPECT_FALSE(slot.has_value());
}

TEST_F(FileOutputBackendFixture, GetLogRecordReturnsObjectSameAsAllocatorWould)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "GetLogRecord can return object same as returned from the allocator.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto fcntl_mock = amp::pmr::make_unique<bmw::os::FcntlMock>(memory_resource_);
    auto unistd_mock = amp::pmr::make_unique<bmw::os::UnistdMock>(memory_resource_);
    FileOutputBackend unit(std::move(message_builder),
                           file_descriptor_,
                           std::move(allocator_),
                           std::move(fcntl_mock),
                           std::move(unistd_mock));

    EXPECT_CALL(*raw_message_builder_mock_, GetNextSpan).WillRepeatedly(Return(OptionalSpan{}));
    const auto slot = unit.ReserveSlot();

    const auto& object = unit.GetLogRecord(slot.value());
    //  Identify as same based on comparing addresses of objects:
    EXPECT_EQ(&object, &(raw_allocator_ptr_->GetUnderlyingBufferFor(slot.value().GetSlotOfSelectedRecorder())));
}

TEST_F(FileOutputBackendFixture, BackendConstructionShallCallNonBlockingFileSetup)
{
    RecordProperty("Requirement", ", 7");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "File backend construction shall return non blocking file setup. The component shall set the "
                   "FD_CLOEXEC (or O_CLOEXEC) flag on all the file descriptor it owns");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    bmw::os::Fcntl::Open flags = bmw::os::Fcntl::Open::kReadWrite;
    auto fcntl_mock = amp::pmr::make_unique<bmw::os::FcntlMock>(memory_resource_);
    auto unistd_mock = amp::pmr::make_unique<bmw::os::UnistdMock>(memory_resource_);
    auto fcntl_mock_raw_ptr = fcntl_mock.get();

    //  Expect call to Fcntl setting Non-Blocking properties of a file:
    EXPECT_CALL(*fcntl_mock_raw_ptr, fcntl(_, bmw::os::Fcntl::Command::kFileGetStatusFlags))
        .Times(1)
        .WillOnce(Return(flags));
    EXPECT_CALL(*fcntl_mock_raw_ptr,
                fcntl(_,
                      bmw::os::Fcntl::Command::kFileSetStatusFlags,
                      flags | bmw::os::Fcntl::Open::kNonBlocking | bmw::os::Fcntl::Open::kCloseOnExec))
        .Times(1);

    //  Given construction
    FileOutputBackend unit(std::move(message_builder),
                           file_descriptor_,
                           std::move(allocator_),
                           std::move(fcntl_mock),
                           std::move(unistd_mock));
}

TEST_F(FileOutputBackendFixture, MissingFlagsShallSkipCallToSetupFile)
{
    RecordProperty("Requirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "File backend construction shall not do file setup if there is any missing flag.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto fcntl_mock = amp::pmr::make_unique<bmw::os::FcntlMock>(memory_resource_);
    auto unistd_mock = amp::pmr::make_unique<bmw::os::UnistdMock>(memory_resource_);
    auto fcntl_mock_raw_ptr = fcntl_mock.get();

    //  Expect call to Fcntl setting Non-Blocking properties of a file:
    EXPECT_CALL(*fcntl_mock_raw_ptr, fcntl(_, bmw::os::Fcntl::Command::kFileGetStatusFlags))
        .Times(1)
        .WillOnce(Return(amp::unexpected<::bmw::os::Error>{::bmw::os::Error::createFromErrno(ENOENT)}));

    EXPECT_CALL(*fcntl_mock_raw_ptr, fcntl(_, _, _)).Times(0);

    //  Given construction
    FileOutputBackend unit(std::move(message_builder),
                           file_descriptor_,
                           std::move(allocator_),
                           std::move(fcntl_mock),
                           std::move(unistd_mock));
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
