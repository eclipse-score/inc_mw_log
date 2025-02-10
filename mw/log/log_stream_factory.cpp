// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#include "platform/aas/mw/log/log_stream_factory.h"

#include "platform/aas/mw/log/detail/thread_local_guard.h"
#include "platform/aas/mw/log/runtime.h"

bmw::mw::log::LogStream bmw::mw::log::detail::LogStreamFactory::GetStream(const LogLevel log_level,
                                                                          const amp::string_view context_id) noexcept
{
    if (not ThreadLocalGuard::IsWithingLogging())
    {
        ThreadLocalGuard guard{};
        // Unnamed object ok, since it will be moved out of this function
        // NOLINTNEXTLINE(bmw-no-unnamed-temporary-objects): See above
        return bmw::mw::log::LogStream{Runtime::GetRecorder(), Runtime::GetFallbackRecorder(), log_level, context_id};
    }
    else
    {
        // Unnamed object ok, since it will be moved out of this function
        // NOLINTNEXTLINE(bmw-no-unnamed-temporary-objects): See above
        return bmw::mw::log::LogStream{
            Runtime::GetFallbackRecorder(), Runtime::GetFallbackRecorder(), log_level, context_id};
    }
}
