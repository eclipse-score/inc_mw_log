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



#ifndef PLATFORM_AAS_MW_LOG_DETAIL_DLT_ARGUMENT_COUNTER_H
#define PLATFORM_AAS_MW_LOG_DETAIL_DLT_ARGUMENT_COUNTER_H

#include "amp_callback.hpp"
#include "platform/aas/mw/log/detail/add_argument_result.h"

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

class DltArgumentCounter
{
  public:
    using add_argument_callback = amp::callback<AddArgumentResult(void), 64UL>;

    explicit DltArgumentCounter(std::uint8_t&) noexcept;
    AddArgumentResult TryAddArgument(add_argument_callback) noexcept;

  private:
    std::uint8_t& counter_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif
