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



#ifndef PLATFORM_AAS_MW_LOG_DETAIL_WAIT_FREE_PRODUCER_QUEUE_LINEAR_CONTROL_BLOCK_H
#define PLATFORM_AAS_MW_LOG_DETAIL_WAIT_FREE_PRODUCER_QUEUE_LINEAR_CONTROL_BLOCK_H

#include "amp_span.hpp"

#include <atomic>
#include <cstdint>
#include <limits>

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

using Byte = char;
//  Use hack to misslead KW
//  using Byte = std::string::value_type;
using Length = std::uint64_t;
using SpanLength = amp::span<Byte>::size_type;

/// \brief Each entry in the buffer will consist of a length prefix followed by payload.
/// \returns the length of the prefix in bytes.
constexpr Length GetLengthOffsetBytes()
{
    return sizeof(Length);
}

constexpr Length GetMaxLinearBufferLengthBytes()
{
    static_assert(sizeof(Length) >= sizeof(SpanLength), "Max of SpanLength should be contained in Length");
    return static_cast<Length>(std::numeric_limits<SpanLength>::max());
}

constexpr Length GetMaxAcquireLengthBytes()
{
    // We need to define an upper bound to define guarantees for avoiding index overflows.
    // Limit could be increased if needed, but for DLT v1 we need at least 64 K.
    return 128UL * 1024UL * 1024UL;
}

constexpr Length GetMaxNumberOfConcurrentWriters()
{
    return 64UL;
}

constexpr Length GetMaxLinearBufferCapacityBytes()
{
    return std::numeric_limits<Length>::max() -
           GetMaxNumberOfConcurrentWriters() * (GetMaxAcquireLengthBytes() + GetLengthOffsetBytes());
}

// ----- COMMON_ARGUMENTATION ----
// Maintaining compatibility and avoiding performance overhead outweighs POD Type (class) based design. The Struct
// is ONLY used internally under the namespace detail and ONLY for data_router sub-dir, it is NOT exposed publicly;
// this is additionally guaranteed by the build system(bazel) visibility. Moreover, the Type is simple and does not
// require invariance (interface OR custom behavior) as per the design.
// ------------------------------

struct LinearControlBlock
{
    // COMMON_ARGUMENTATION
    // 
    amp::span<Byte> data{};
    // COMMON_ARGUMENTATION
    // 
    std::atomic<Length> acquired_index{};
    // COMMON_ARGUMENTATION
    // 
    std::atomic<Length> written_index{};
    // COMMON_ARGUMENTATION
    // 
    std::atomic<Length> number_of_writers{};
};

/// \returns true if number_of_bytes fits in control_block at the offset.
bool DoBytesFitInRemainingCapacity(const amp::span<Byte>& buffer,
                                   const Length offset,
                                   const Length number_of_bytes) noexcept;

/// \returns the size of the span casted to Length
Length GetDataSizeAsLength(const amp::span<Byte>& data) noexcept;

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif
