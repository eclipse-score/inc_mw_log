// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#ifndef PLATFORM_AAS_MW_LOG_ERROR_H
#define PLATFORM_AAS_MW_LOG_ERROR_H

#include "platform/aas/lib/result/error.h"

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

enum class Error : bmw::result::ErrorCode
{
    kUnknownError = 0,
    kInvalidLogLevelString,
    kInvalidLogModeString,
    kConfigurationFilesNotFound,
    kConfigurationOptionalJsonKeyNotFound,
    kMaximumNumberOfRecordersExceeded,
    kRecorderFactoryUnsupportedLogMode,
    kNoLogModeSpecified,
    kReceiverInitializationError,
    kUnlinkSharedMemoryError,
    kFailedToSendMessageToDatarouter,
    kFailedToSetLoggerThreadName,
    kSetSharedMemoryPermissionsError,
    kShutdownDuringInitialization,
    kSloggerError,
    kLogFileCreationFailed,
    kBlockingTerminationSignalFailed,
    kMemoryResourceError,
};

class ErrorDomain final : public bmw::result::ErrorDomain
{
    amp::string_view MessageFor(const bmw::result::ErrorCode& code) const noexcept override;
};

bmw::result::Error MakeError(const bmw::mw::log::detail::Error code, const std::string_view user_message = "") noexcept;

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
#endif  // PLATFORM_AAS_MW_LOG_ERROR_H
