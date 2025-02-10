// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#ifndef PLATFORM_AAS_MW_LOG_DETAIL_INITIALIZATION_REPORTER_H
#define PLATFORM_AAS_MW_LOG_DETAIL_INITIALIZATION_REPORTER_H

#include "platform/aas/lib/result/error.h"

#include "amp_optional.hpp"

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

/// \brief Provides user feedback in case of errors during initialization of the logging library.
/// \details During the initialization of logging we need a way to report errors to the user, for example if something
/// is wrong in the configuration files. As "regular" logging is not available at this point, we need to define an
/// alternative mechanism.
void ReportInitializationError(const bmw::result::Error& error,
                               const amp::string_view context_info = "",
                               const amp::optional<amp::string_view> app_id = {}) noexcept;

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif
