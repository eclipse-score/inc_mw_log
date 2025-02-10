// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#ifndef PLATFORM_AAS_MW_LOG_TEST_MY_CUSTOM_LIB_MY_CUSTOM_TYPE_ARA_LOG_H
#define PLATFORM_AAS_MW_LOG_TEST_MY_CUSTOM_LIB_MY_CUSTOM_TYPE_ARA_LOG_H

#include "platform/aas/ara/log/inc/ara/log/logstream.h"
#include "platform/aas/mw/log/test/my_custom_lib/my_custom_type.h"

namespace my
{
namespace custom
{
namespace type
{

ara::log::LogStream& operator<<(ara::log::LogStream& log_stream,
                                const my::custom::type::MyCustomType& my_custom_type) noexcept;

}  // namespace type
}  // namespace custom
}  // namespace my

#endif  // PLATFORM_AAS_MW_LOG_TEST_MY_CUSTOM_LIB_MY_CUSTOM_TYPE_ARA_LOG_H
