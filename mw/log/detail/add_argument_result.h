// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************




#ifndef PLATFORM_AAS_MW_LOG_DETAIL_ADD_ARGUMENT_RESULT_H
#define PLATFORM_AAS_MW_LOG_DETAIL_ADD_ARGUMENT_RESULT_H

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

enum class AddArgumentResult : bool
{
    NotAdded = false,
    Added = true,
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif
