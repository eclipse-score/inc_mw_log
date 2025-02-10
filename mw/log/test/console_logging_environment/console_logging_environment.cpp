// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#include "platform/aas/mw/log/test/console_logging_environment/console_logging_environment.h"
#include "platform/aas/mw/log/detail/recorder_factory.h"
#include "platform/aas/mw/log/runtime.h"

namespace bmw
{
namespace mw
{
namespace log
{

void ConsoleLoggingEnvironment::SetUp()
{
    bmw::mw::log::detail::Configuration config{};
    config.SetLogMode({bmw::mw::LogMode::kConsole});
    config.SetDefaultConsoleLogLevel(bmw::mw::log::LogLevel::kVerbose);
    recorder_ = bmw::mw::log::detail::RecorderFactory().CreateRecorderFromLogMode(bmw::mw::LogMode::kConsole, config);

    bmw::mw::log::detail::Runtime::SetRecorder(recorder_.get());
}

void ConsoleLoggingEnvironment::TearDown()
{
    bmw::mw::log::detail::Runtime::SetRecorder(nullptr);
    recorder_.reset();
}

}  // namespace log
}  // namespace mw
}  // namespace bmw
