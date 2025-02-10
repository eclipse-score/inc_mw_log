// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#ifndef MW_LOG_DETAILS_SLOT_DRAINER_H_
#define MW_LOG_DETAILS_SLOT_DRAINER_H_

#include "platform/aas/mw/log/detail/circular_allocator.h"
#include "platform/aas/mw/log/detail/file_logging/imessage_builder.h"
#include "platform/aas/mw/log/detail/file_logging/non_blocking_writer.h"
#include "platform/aas/mw/log/detail/log_record.h"
#include "platform/aas/mw/log/slot_handle.h"

#include <amp_circular_buffer.hpp>
#include <amp_span.hpp>

#include <memory>
#include <mutex>

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

class SlotDrainer
{
  public:
    enum class FlushResult : std::uint8_t
    {
        kAllDataProcessed = 0,
        kPartiallyProcessed,
        kNumberOfProcessedSlotsExceeded,
    };
    SlotDrainer(std::unique_ptr<IMessageBuilder> message_builder,
                std::unique_ptr<CircularAllocator<LogRecord>>& allocator,
                const std::int32_t file_descriptor,
                amp::pmr::unique_ptr<bmw::os::Unistd> unistd,
                const std::size_t limit_slots_in_one_cycle = 32UL);

    SlotDrainer(SlotDrainer&&) noexcept = delete;
    SlotDrainer(const SlotDrainer&) noexcept = delete;
    SlotDrainer& operator=(SlotDrainer&&) noexcept = delete;
    SlotDrainer& operator=(const SlotDrainer&) noexcept = delete;

    void PushBack(const SlotHandle& slot) noexcept;
    void Flush() noexcept;

    ~SlotDrainer();

  private:
    amp::expected<FlushResult, bmw::mw::log::detail::Error> TryFlushSlots() noexcept;
    amp::expected<FlushResult, bmw::mw::log::detail::Error> TryFlushSpans() noexcept;
    bool MoreSlotsAvailableAndLoaded() noexcept;
    bool MoreSpansAvailableAndLoaded() noexcept;

    std::unique_ptr<CircularAllocator<LogRecord>>& allocator_;
    std::unique_ptr<IMessageBuilder> message_builder_;
    std::mutex context_mutex_;
    static constexpr std::size_t kMaxCircularBufferSize = 1024UL;
    //  TODO: assert size of circular_buffer:
    amp::circular_buffer<SlotHandle, kMaxCircularBufferSize> circular_buffer_;
    //  To manually release resource and to set and reset access:
    amp::optional<std::reference_wrapper<const SlotHandle>> current_slot_;
    NonBlockingWriter non_blocking_writer_;
    const std::size_t limit_slots_in_one_cycle_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  //  MW_LOG_DETAILS_SLOT_DRAINER_H_
