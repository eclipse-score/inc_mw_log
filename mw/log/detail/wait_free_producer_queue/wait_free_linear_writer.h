// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************




#ifndef PLATFORM_AAS_MW_LOG_DETAIL_WAIT_FREE_LINEAR_WRITER_H
#define PLATFORM_AAS_MW_LOG_DETAIL_WAIT_FREE_LINEAR_WRITER_H

#include "platform/aas/mw/log/detail/wait_free_producer_queue/linear_control_block.h"

#include "amp_callback.hpp"
#include "amp_optional.hpp"
#include "amp_span.hpp"

#include <atomic>

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

struct AcquiredData
{
    // Maintaining compatibility and avoiding performance overhead outweighs POD Type (class) based design. The Struct
    // is ONLY used internally under the namespace detail and ONLY for data_router sub-dir, it is NOT exposed publicly;
    // this is additionally guaranteed by the build system(bazel) visibility. Moreover, the Type is simple and does not
    // require invariance (interface OR custom behavior) as per the design.
    // 
    amp::span<Byte> data;
};

class WaitFreeLinearWriter;
using PreAcquireHook = amp::callback<void(WaitFreeLinearWriter&)>;

/// \brief Wait-free writing to a linear buffer.
/// Thread-safe for multiple writers.
/// No overwriting of data.
/// First in first out.
/// PreAcquireHook is used for testing certain call sequences, default constructed as no-op in production.
class WaitFreeLinearWriter
{
  public:
    explicit WaitFreeLinearWriter(LinearControlBlock& cb,
                                  PreAcquireHook pre_acquire_hook = std::move(empty_hook)) noexcept;

    /// \brief Try to acquire the length for writing.
    /// Returns empty if there is not enough space available.
    amp::optional<AcquiredData> Acquire(const Length length) noexcept;

    /// \brief Release the acquired data.
    void Release(const AcquiredData& acquired_data) noexcept;

  private:
    static inline void empty_hook(WaitFreeLinearWriter&) noexcept {}
    
  private:
    
    
    LinearControlBlock& control_block_;
    PreAcquireHook pre_acquire_hook_;
    
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif
