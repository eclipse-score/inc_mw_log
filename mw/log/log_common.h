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



#ifndef PLATFORM_AAS_MW_LOG_COMMON_H
#define PLATFORM_AAS_MW_LOG_COMMON_H

#include <amp_span.hpp>

#include <cstdint>

#include "amp_string_view.hpp"
#include <chrono>
#include <ratio>
#include <typeindex>
#include <unordered_map>

namespace bmw
{
namespace mw
{
namespace log
{

namespace detail
{
using PeriodSuffixMap = std::unordered_map<std::type_index, amp::string_view>;

extern const PeriodSuffixMap PeriodToSuffix;
}  // namespace detail

template <typename Period>
amp::string_view DurationUnitSuffix() noexcept
{
    return detail::PeriodToSuffix.at(typeid(typename Period::type));
}

}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif