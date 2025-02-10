// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#ifndef PLATFORM_AAS_MW_LOG_DETAIL_LOGGING_IDENTIFIER_H
#define PLATFORM_AAS_MW_LOG_DETAIL_LOGGING_IDENTIFIER_H

#include "amp_string_view.hpp"

#include "visitor/visit_as_struct.h"

#include <array>
#include <functional>


namespace bmw

{
namespace mw
{
namespace log
{
namespace detail
{


/// \brief Contains a 4 byte identifier that can be used for ECU, application or context identifiers.
class LoggingIdentifier
{
  public:
    /// \brief Crops the given string view to a maximum of kMaxLength bytes.
    explicit LoggingIdentifier(const amp::string_view) noexcept;

    /// \brief Returns the underlying fixed-length string view.
    amp::string_view GetStringView() const noexcept;

    /// \brief Use this to enable LoggingIdentifier as a key in a map.
    class HashFunction
    {
      public:
        size_t operator()(const LoggingIdentifier& id) const;
    };

    friend bool operator==(const LoggingIdentifier&, const LoggingIdentifier&) noexcept;
    friend bool operator!=(const LoggingIdentifier&, const LoggingIdentifier&) noexcept;

    LoggingIdentifier() noexcept = default;
    LoggingIdentifier& operator=(const LoggingIdentifier& rhs) noexcept = default;
    LoggingIdentifier& operator=(LoggingIdentifier&& rhs) noexcept = default;
    LoggingIdentifier(const LoggingIdentifier&) noexcept = default;
    LoggingIdentifier(LoggingIdentifier&&) noexcept = default;
    ~LoggingIdentifier() noexcept = default;

    /// \brief This maximum is fixed to 4 bytes by the DLT protocol standard.
    constexpr static std::size_t kMaxLength{4UL};

    // This variable is public because of necessity to pass it to STRUCT_VISITABLE in log_entry.h
    std::array<amp::string_view::value_type, kMaxLength> data_{};
};




/* (1) False positive: Line contains a single statement. (2) No unused stuff. (3) Expected.*/

// NOLINTBEGIN(bmw-struct-usage-compliance) justified by design
// Forward declaration for struct_visitable_impl is required for implementation
// std::forward<T>(s) added due to CB-#10171555
// 
// 
STRUCT_VISITABLE(LoggingIdentifier, data_)

// NOLINTEND(bmw-struct-usage-compliance) justified by design




}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  // PLATFORM_AAS_MW_LOG_DETAIL_RUNTIME_H
