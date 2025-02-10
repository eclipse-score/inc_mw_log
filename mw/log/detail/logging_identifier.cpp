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


#include "platform/aas/mw/log/detail/logging_identifier.h"

#include "amp_utility.hpp"
#include <cstring>

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

LoggingIdentifier::LoggingIdentifier(const amp::string_view identifier) noexcept
{
    data_.fill(0U);
    amp::ignore = std::copy_n(identifier.begin(), std::min(identifier.size(), data_.size()), data_.begin());
}

amp::string_view LoggingIdentifier::GetStringView() const noexcept
{
    return amp::string_view{data_.data(), strnlen(data_.data(), kMaxLength)};
}

std::size_t LoggingIdentifier::HashFunction::operator()(const LoggingIdentifier& id) const
{
    std::int32_t value{};
    static_assert(sizeof(value) == sizeof(LoggingIdentifier::data_), "data_ must have the correct size");
    // NOLINTNEXTLINE(bmw-banned-function) memcpy is needed
    amp::ignore = std::memcpy(&value, id.data_.data(), id.data_.size());
    return std::hash<std::int32_t>{}(value);
}

bool operator==(const LoggingIdentifier& lhs, const LoggingIdentifier& rhs) noexcept
{
    return std::equal(lhs.data_.begin(), lhs.data_.end(), rhs.data_.begin());
}

bool operator!=(const LoggingIdentifier& lhs, const LoggingIdentifier& rhs) noexcept
{
    return !(lhs == rhs);
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
