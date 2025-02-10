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


/// Only testing additional corner cases here, the Reader is mostly already tested through behavior tests in
/// wait_free_linear_writer.cpp.

#include "platform/aas/mw/log/detail/wait_free_producer_queue/linear_control_block.h"

#include "amp_utility.hpp"

#include <gtest/gtest.h>

#include <vector>

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

TEST(LinearControlBlock, LengthExceedingMaxThresholdShouldReturnTruncated)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "The helper function shall check if the length and offset are valid");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    constexpr auto kBufferSize = 5ul;
    std::vector<bmw::mw::log::detail::Byte> buffer(kBufferSize);
    auto data = amp::span<bmw::mw::log::detail::Byte>(buffer.data(),
                                                      static_cast<bmw::mw::log::detail::SpanLength>(buffer.size()));

    const auto invalid_offset = kBufferSize + 1ul;
    constexpr auto kArbitraryBytesCount = 2ul;

    ASSERT_FALSE(DoBytesFitInRemainingCapacity(data, invalid_offset, kArbitraryBytesCount));
}

TEST(LinearControlBlockTests, BytesShallNotFitInRemainingCCapacityIfOffsetBiggerThanTheBufferSize)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verify that the bytes can't fit in the remaining capacity in case the offset is bigger than the buffer size.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When offset is biggeer than the buffer size.
    const amp::span<bmw::mw::log::detail::Byte> buffer;
    constexpr bmw::mw::log::detail::Length kOffsetBiggerThanBufferSize{10U};

    constexpr bmw::mw::log::detail::Length kSingleByte{1U};

    // Shall return false.
    EXPECT_FALSE(bmw::mw::log::detail::DoBytesFitInRemainingCapacity(buffer, kOffsetBiggerThanBufferSize, kSingleByte));
}

}  // namespace

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
