// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************




#include "platform/aas/mw/log/detail/dlt_argument_counter.h"

#include <limits>

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{


DltArgumentCounter::DltArgumentCounter(std::uint8_t& counter) noexcept : counter_(counter) {}

AddArgumentResult DltArgumentCounter::TryAddArgument(add_argument_callback callback) noexcept
{
    
    constexpr uint8_t MAX_COUNTER = std::numeric_limits<std::remove_reference<decltype(counter_)>::type>::max();
    if (counter_ == MAX_COUNTER)
    {
        return AddArgumentResult::NotAdded;
    }

    const auto result = callback();
    if (result == AddArgumentResult::Added)
    {
        counter_++;
    }

    return result;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
