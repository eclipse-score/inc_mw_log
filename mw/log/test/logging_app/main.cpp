// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#include "platform/aas/test/pas/logging/utils/src/logging_app.h"

#include "platform/aas/mw/lifecycle/runapplication.h"

#if !defined(USE_MIDDLEWARE_LOGGING)
#error "Define logging framework to be tested"
#endif

#if USE_MIDDLEWARE_LOGGING
#include "platform/aas/mw/log/logging.h"
namespace logUnderTest = bmw::mw::log;
#else
#include "ara/log/logging.h"
namespace logUnderTest = ara::log;
#endif

namespace
{
using TypeFunction = logUnderTest::LogStream (&)(void);
}



int main(std::int32_t argc, const char* argv[])


{
#if defined(__GNUC__) && not defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"  //  Cast is used when functions have overloads provided by mw::log.
                                                 //  Bigger harm would be to use symbols to avoid cast in other cases.
#endif                                           // #if defined(__GNUC__) && not defined(__clang__)

    
    return bmw::mw::lifecycle::run_application<bmw::logging::example::LoggingApp<TypeFunction>>(
        argc,
        argv,
        static_cast<TypeFunction>(logUnderTest::LogInfo),
        static_cast<TypeFunction>(logUnderTest::LogDebug),
        static_cast<TypeFunction>(logUnderTest::LogWarn),
        static_cast<TypeFunction>(logUnderTest::LogError),
        static_cast<TypeFunction>(logUnderTest::LogFatal));
    

#if defined(__GNUC__) && not defined(__clang__)
#pragma GCC diagnostic pop
#endif  // #if defined(__GNUC__) && not defined(__clang__)
}
