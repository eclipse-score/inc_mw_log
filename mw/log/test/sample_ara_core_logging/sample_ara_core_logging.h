// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#ifndef PLATFORM_AAS_MW_LOG_TEST_SAMPLE_ARA_CORE_LOGGING_SAMPLE_ARA_CORE_LOGGING_H
#define PLATFORM_AAS_MW_LOG_TEST_SAMPLE_ARA_CORE_LOGGING_SAMPLE_ARA_CORE_LOGGING_H

#include "platform/aas/mw/log/log_stream.h"

#include <amp_string_view.hpp>
#include <ara/core/result.h>

namespace ara
{
namespace core
{

template <typename T, typename E>
bmw::mw::log::LogStream& operator<<(bmw::mw::log::LogStream& log_stream, const ara::core::Result<T, E>& result) noexcept
{
    if (result.HasValue())
    {
        log_stream << "Result value: " << result.Value();
    }
    else
    {
        log_stream << "Error message: ";

        const auto& msg = result.Error().UserMessage();
        if (msg.size())
        {
            log_stream << amp::string_view{msg.data(), msg.size()};
        }
        else
        {
            log_stream << "{EMPTY}";
        }
    }

    return log_stream;
}

}  // namespace core
}  // namespace ara

#endif  // PLATFORM_AAS_MW_LOG_TEST_SAMPLE_ARA_CORE_LOGGING_SAMPLE_ARA_CORE_LOGGING_H
