// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#include "platform/aas/mw/log/detail/error.h"

amp::string_view bmw::mw::log::detail::ErrorDomain::MessageFor(const bmw::result::ErrorCode& code) const noexcept
{
    // boundary check is done by default case in switch
    // 
    switch (static_cast<bmw::mw::log::detail::Error>(code))
    {
        case Error::kInvalidLogLevelString:
            return "The string does not contain a valid log level.";
        case Error::kInvalidLogModeString:
            return "The string does not contain a valid log mode.";
        case Error::kConfigurationFilesNotFound:
            return "No logging configuration files could be found.";
        case Error::kConfigurationOptionalJsonKeyNotFound:
            return "Configuration key not found in JSON file.";
        case Error::kMaximumNumberOfRecordersExceeded:
            return "Exceeded the maximum number of active recorders.";
        case Error::kRecorderFactoryUnsupportedLogMode:
            return "Unsupported LogMode encountered in the RecorderFactory, using EmptyRecorder instead.";
        case Error::kNoLogModeSpecified:
            return "No log mode in configuration, using EmptyRecorder instead.";
        case Error::kReceiverInitializationError:
            return "Failed to initialize message passing receiver";
        case Error::kUnlinkSharedMemoryError:
            return "Failed to unlink shared memory file. Memory might be leaked.";
        case Error::kFailedToSendMessageToDatarouter:
            return "Failed to send message to Datarouter. Logging is shutting down.";
        case Error::kFailedToSetLoggerThreadName:
            return "Failed to set thread name of logger thread";
        case Error::kSetSharedMemoryPermissionsError:
            return "Failed to change ownership of shared memory file.";
        case Error::kShutdownDuringInitialization:
            return "Shutdown was requested during initialization of logging library.";
        case Error::kSloggerError:
            return "The slogger2 library returned an error.";
        case Error::kLogFileCreationFailed:
            return "Failed to create the log file.";
        case Error::kBlockingTerminationSignalFailed:
            return "Failed to block termination signal.";
        case Error::kMemoryResourceError:
            return "Failed to get memory resource   .";
        case Error::kUnknownError:
        default:
            return "Unknown Error";
    }
}

namespace
{
constexpr bmw::mw::log::detail::ErrorDomain mw_log_error_domain;
}

bmw::result::Error bmw::mw::log::detail::MakeError(const bmw::mw::log::detail::Error code,
                                                   const std::string_view user_message) noexcept
{
    return {static_cast<bmw::result::ErrorCode>(code), mw_log_error_domain, user_message};
}
