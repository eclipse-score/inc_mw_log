// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#include "platform/aas/mw/log/log_level.h"

#include "gtest/gtest.h"

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

TEST(LogLevelTesting, EnsureMaxLevelCoversAllEnumCases)
{
    const bmw::mw::log::LogLevel max_log_level = GetMaxLogLevelValue();
    //  Test conditions are intentionally put into switch to enforce covering all enum values:
    switch (max_log_level)
    {
        case LogLevel::kVerbose:
            EXPECT_EQ(LogLevel::kVerbose, max_log_level);
            break;
        case LogLevel::kDebug:
        case LogLevel::kInfo:
        case LogLevel::kWarn:
        case LogLevel::kError:
        case LogLevel::kFatal:
        case LogLevel::kOff:
        default:
            FAIL();
            break;
    }
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
