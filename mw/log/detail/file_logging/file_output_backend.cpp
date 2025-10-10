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
#include "platform/aas/mw/log/detail/log_record.h"
#include "platform/aas/mw/log/slot_handle.h"

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

FileOutputBackend::FileOutputBackend(std::unique_ptr<IMessageBuilder> message_builder,
                                     const std::int32_t file_descriptor,
                                     std::unique_ptr<CircularAllocator<LogRecord>> allocator,
                                     amp::pmr::unique_ptr<bmw::os::Fcntl> fcntl_instance,
                                     amp::pmr::unique_ptr<bmw::os::Unistd> unistd) noexcept
    : Backend(),
      buffer_allocator_(std::move(allocator)),
      slot_drainer_(std::move(message_builder), buffer_allocator_, file_descriptor, std::move(unistd))
{
    const auto flags = fcntl_instance->fcntl(file_descriptor, bmw::os::Fcntl::Command::kFileGetStatusFlags);
    if (flags.has_value())
    {
        amp::ignore = fcntl_instance->fcntl(
            file_descriptor,
            bmw::os::Fcntl::Command::kFileSetStatusFlags,
            flags.value() | bmw::os::Fcntl::Open::kNonBlocking | bmw::os::Fcntl::Open::kCloseOnExec);
    }
}

amp::optional<SlotHandle> FileOutputBackend::ReserveSlot() noexcept
{
    slot_drainer_.Flush();
    const auto slot = buffer_allocator_->AcquireSlotToWrite();

    if (slot.has_value())
    {
        //  CircularAllocator has capacity limited by CheckFoxMaxCapacity thus the cast is valid:
        return SlotHandle{static_cast<SlotIndex>(slot.value())};
    }
    
    return {};
    
}

void FileOutputBackend::FlushSlot(const SlotHandle& slot) noexcept
{
    slot_drainer_.PushBack(slot);
    slot_drainer_.Flush();
}

LogRecord& FileOutputBackend::GetLogRecord(const SlotHandle& slot) noexcept
{
    //  Cast to bigger integer type:
    return buffer_allocator_->GetUnderlyingBufferFor(static_cast<std::size_t>(slot.GetSlotOfSelectedRecorder()));
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
