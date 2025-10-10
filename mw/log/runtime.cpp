/********************************************************************************
* Copyright (c) 2025 Contributors to the Eclipse Foundation
*
* See the NOTICE file(s) distributed with this work for additional
* information regarding copyright ownership.
*
* This program and the accompanying materials are made available under the
* terms of the Apache License Version 2.0 which is available at
* https://www.apache.org/licenses/LICENSE-2.0
*
* SPDX-License-Identifier: Apache-2.0
********************************************************************************/


#include "platform/aas/mw/log/runtime.h"

#include "platform/aas/mw/log/detail/thread_local_guard.h"
#include "platform/aas/mw/log/irecorder_factory.h"

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{


Runtime& Runtime::Instance(Recorder* const recorder) noexcept

{
    
    static Runtime runtime{recorder};
    
    return runtime;
}

Runtime::Runtime(Recorder* const recorder) noexcept
{
    ThreadLocalGuard guard{};
    const auto recorder_factory = CreateRecorderFactory();
    if (recorder == nullptr)
    {
        default_recorder_ = recorder_factory->CreateFromConfiguration(amp::pmr::get_default_resource());
    }
    else
    {
        default_recorder_ = recorder_factory->CreateWithConsoleLoggingOnly(amp::pmr::get_default_resource());
        recorder_instance_ = recorder;
    }
}

bmw::mw::log::Recorder& Runtime::GetRecorder() noexcept
{
    const auto& instance = Instance(nullptr);
    if (instance.recorder_instance_ != nullptr)
    {
        return *instance.recorder_instance_;
    }
    return *instance.default_recorder_;
}

bmw::mw::log::Recorder& Runtime::GetFallbackRecorder() noexcept
{
    static std::unique_ptr<Recorder> recorder{
        CreateRecorderFactory()->CreateWithConsoleLoggingOnly(amp::pmr::get_default_resource())};
    return *recorder;
}

bmw::mw::log::LoggerContainer& Runtime::GetLoggerContainer() noexcept
{
    return Instance(nullptr).logger_container_instance_;
}

void Runtime::SetRecorder(Recorder* const recorder) noexcept
{
    Instance(recorder).recorder_instance_ = recorder;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
