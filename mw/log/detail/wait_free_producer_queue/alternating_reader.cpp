// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#include "platform/aas/mw/log/detail/wait_free_producer_queue/alternating_reader.h"

#include <thread>

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

AlternatingReaderProxy::AlternatingReaderProxy(AlternatingControlBlock& dcb) noexcept
    : alternating_control_block_(dcb),
      previous_logging_ipc_counter_value_{dcb.switch_count_points_active_for_writing.load()}
{
}

AlternatingReadOnlyReader::AlternatingReadOnlyReader(const AlternatingControlBlock& dcb,
                                                     const amp::span<Byte> buffer_even,
                                                     const amp::span<Byte> buffer_odd) noexcept
    : alternating_control_block_(dcb), reader_({}), buffer_even_{buffer_even}, buffer_odd_{buffer_odd}
{
}

LinearReader AlternatingReadOnlyReader::CreateLinearReader(std::uint32_t block_id_count) noexcept
{
    auto block_id = SelectLinearControlBlockId(block_id_count);
    auto& block = SelectLinearControlBlockReference(block_id, alternating_control_block_);

    const auto written_bytes = block.written_index.load();

    auto& buffer = block_id == AlternatingControlBlockSelectId::kBlockEven ? buffer_even_ : buffer_odd_;
    return CreateLinearReaderFromDataAndLength(buffer, written_bytes);
}

auto GetSplitBlocks(AlternatingControlBlockSelectId block_id_active_for_writing,
                    AlternatingControlBlock& alternating_control_block)
{
    return std::tuple{ReusedCleanupBlockReference{SelectLinearControlBlockReference(
                          GetOppositeLinearControlBlock(block_id_active_for_writing), alternating_control_block)},
                      TerminatingBlockReference{
                          SelectLinearControlBlockReference(block_id_active_for_writing, alternating_control_block)}};
}

///  Assumption: The Switch method shall not be called from a concurrent contexts i.e. it supports single consumer.
std::uint32_t AlternatingReaderProxy::Switch() noexcept
{
    const auto switch_count_points_active_for_writing =
        alternating_control_block_.switch_count_points_active_for_writing.load();

    //  Sanity check: Relevant part of shared memory data context shall not be modified outside of this function:
    //  TODO:  Handle or report error. This is most likely a fatal error
    //  AMP_ASSERT(previous_logging_ipc_counter_value_ == switch_count_points_active_for_writing);

    auto block_id_active_for_writing = SelectLinearControlBlockId(switch_count_points_active_for_writing);

    auto [restarting_control_block, terminating_control_block_intermediate] =
        GetSplitBlocks(block_id_active_for_writing, alternating_control_block_);

    std::ignore = terminating_control_block_intermediate;

    //  Reset counters for writing new data into restarting block.
    const auto acquired_index = restarting_control_block.GetReusedCleanupBlock().acquired_index.exchange(0);
    const auto written_index = restarting_control_block.GetReusedCleanupBlock().written_index.exchange(0);
    //  TODO: , handle or report error. Reader shall work on completely written, because ASIL-B clients shall
    //  not just abandon the work (exceptions at initialization must be considered) AMP_ASSERT(acquired_index ==
    //  written_index);
    std::ignore = acquired_index;
    std::ignore = written_index;

    // Switch the active buffer for future writers.
    const auto save_switch_count = alternating_control_block_.switch_count_points_active_for_writing.fetch_add(1UL);

    //  TODO:  Handle fatal error. No one else shall increment switch counter:
    //  AMP_ASSERT(save_switch_count == switch_count_points_active_for_writing);

    std::atomic_thread_fence(std::memory_order_release);

    // Writer switch may be incomplete. It is not yet safe to read the data in the buffer.
    // It is left as reader responsibility to check if writers released buffer.

    previous_logging_ipc_counter_value_ = save_switch_count + 1;
    return save_switch_count;
}

bool AlternatingReadOnlyReader::IsBlockReleasedByWriters(const std::uint32_t block_id_count) const noexcept
{
    auto block_id = SelectLinearControlBlockId(block_id_count);
    auto& block = SelectLinearControlBlockReference(block_id, alternating_control_block_);

    const bool result =
        (block.number_of_writers.load() == 0) && (block.written_index.load() == block.acquired_index.load());
    if (result)
    {
        std::atomic_thread_fence(std::memory_order_acquire);
    }
    return result;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
