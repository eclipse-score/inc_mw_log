// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#ifndef PLATFORM_AAS_MW_LOG_RUNTIME_H
#define PLATFORM_AAS_MW_LOG_RUNTIME_H

#include "platform/aas/mw/log/logger_container.h"
#include "platform/aas/mw/log/recorder.h"

namespace bmw
{
namespace mw
{
namespace log
{

namespace detail
{

class RecorderFactory;

/// \brief The runtime is a singleton that is responsible for providing the usage of the current Recorder. Due to the
/// distributed nature of the logging library, we need a central place to know which logging infrastructure to use. This
/// enables us logging distributed within our code base without the necessity to inject it everywhere.
///
/// \details We thought about not providing such a runtime and instead injecting the necessary logging recorder
/// everywhere. The main reason against this approach are:
/// 1) cluttering of dependencies. We need logging everywhere, so it would be a big dependency mess
/// 2) We have currently logging in a distributed manner, refactoring to an injecting pattern would require tremendous
/// efforts in the overall codebase

class Runtime final
{
  public:
    /// \brief Implements a singleton, so no copying or moving
    Runtime(const Runtime&) noexcept = delete;
    Runtime(Runtime&&) noexcept = delete;
    Runtime& operator=(const Runtime&) noexcept = delete;
    Runtime& operator=(Runtime&&) noexcept = delete;

    ~Runtime() = default;
    /// \brief Getter function for the currently configured recorder
    ///
    /// \details Be careful, you cannot use GetRecorder() and SetRecorder() in a thread-safe manner. We decided against
    /// a synchronisation primitive since in production code has no real use-case for usage of SetRecorder() and we want
    /// to avoid the cost of the synchronization for production code if race conditions only exists for testing. This
    /// also means that it is safe to invoke `GetRecorder()` concurrently.
    /// \return pointer to either the default recorder or the last set recorder
    static Recorder& GetRecorder() noexcept;

    /// \brief Inject a Recorder into the logging framework
    ///
    /// \param recorder The recorder that from now shall be used by any logging call
    ///
    /// \details Be careful, you cannot use GetRecorder() and SetRecorder() in a thread-safe manner. We decided against
    /// a synchronisation primitive since in production code no real use-case for SetRecorder() exists and we want to
    /// avoid the cost of the synchronization for production code if only exists for testing.
    ///
    /// Be advised that it is the responsibility of the user of this API, to ensure that the set recorder is alive as
    /// long as any other functionality might invoke logging statements!
    static void SetRecorder(Recorder* const recorder) noexcept;

    static Recorder& GetFallbackRecorder() noexcept;

    static bmw::mw::log::LoggerContainer& GetLoggerContainer() noexcept;

    
  private:
    
    explicit Runtime(Recorder* const recorder) noexcept;
    static Runtime& Instance(Recorder* const recorder) noexcept;
    
    /* False positive. There is no initialization list in constructor definition. */
    
    LoggerContainer logger_container_instance_{};

    Recorder* recorder_instance_ = nullptr;
    std::unique_ptr<Recorder> default_recorder_ = nullptr;
    std::unique_ptr<Recorder> fallback_recorder_ = nullptr;
    
    /* False positive. There is no initialization list in constructor definition. */
    
};



}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  // PLATFORM_AAS_MW_LOG_RUNTIME_H
