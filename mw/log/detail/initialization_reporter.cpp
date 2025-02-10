// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#include "platform/aas/mw/log/detail/initialization_reporter.h"

#include "platform/aas/mw/log/detail/error.h"

#include <amp_utility.hpp>

#include <iostream>

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

constexpr bool kVerboseReporting{false};

std::ostream& BeginInitReportStream() noexcept
{
    return std::cerr << "mw::log ";
}

bool IsErrorVerbose(const bmw::result::Error& error) noexcept
{
    if (error == Error::kConfigurationOptionalJsonKeyNotFound)
    {
        // Do not bother the user about missing optional entries.
        return true;
    }

    return false;
}

}  // namespace

void ReportInitializationError(const bmw::result::Error& error,
                               const amp::string_view context_info,
                               const amp::optional<amp::string_view> app_id) noexcept
{
    if ((kVerboseReporting == false) && IsErrorVerbose(error))
    {
        return;
    }

    auto& error_stream = BeginInitReportStream() << "initialization error: " << error;

    if (app_id.has_value())
    {
        error_stream << " for app ";
        std::ignore = error_stream.write(app_id.value().data(), static_cast<std::streamsize>(app_id.value().size()));
    }

    if (context_info.size() > 0)
    {
        error_stream << " with context information: ";
        std::ignore = error_stream.write(context_info.data(), static_cast<std::streamsize>(context_info.size()));
    }
    error_stream << '\n';
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
