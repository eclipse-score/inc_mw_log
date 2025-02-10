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


#ifndef PLATFORM_AAS_MW_LOG_IRECORDER_FACTORY_H
#define PLATFORM_AAS_MW_LOG_IRECORDER_FACTORY_H

#include "platform/aas/mw/log/recorder.h"

#include <memory>

namespace bmw
{
namespace mw
{
namespace log
{


/* False positive 'noexcept' is a keyword and not an identifier which could be hidden */

class IRecorderFactory
{
  public:
    /// \brief Instantiates the recorder(s) according to the configuration files provides by the user
    /// \details Depending on the configuration provided by the user it will instantiate a recorder for DLT, Console,
    /// and/or File logging.
    /// If no configuration can be found, it will fall back to CreateWithConsoleLoggingOnly().
    virtual std::unique_ptr<Recorder> CreateFromConfiguration(
        amp::pmr::memory_resource* memory_resource) const noexcept = 0;

    /// \brief Instantiate a recorder that provides basic console logging.
    /// \details This is for users that do not need or want to provide a logging configuration file. A typical use case
    /// might be for example for unit or component testing.
    virtual std::unique_ptr<Recorder> CreateWithConsoleLoggingOnly(
        amp::pmr::memory_resource* memory_resource) const noexcept = 0;

    /// \brief Instantiates a stub recorder that drops all the logs.
    /// \details For users that want to completely turn of logging for whatever reason.
    virtual std::unique_ptr<Recorder> CreateStub() const noexcept = 0;

    IRecorderFactory() noexcept = default;
    IRecorderFactory(IRecorderFactory&&) noexcept = delete;
    IRecorderFactory(const IRecorderFactory&) noexcept = delete;
    IRecorderFactory& operator=(IRecorderFactory&&) noexcept = delete;
    IRecorderFactory& operator=(const IRecorderFactory&) noexcept = delete;

    virtual ~IRecorderFactory() = default;
};



namespace detail
{
std::unique_ptr<IRecorderFactory> CreateRecorderFactory() noexcept;
}
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  // PLATFORM_AAS_MW_LOG_IRECORDER_FACTORY_H
