// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#ifndef PLATFORM_AAS_MW_LOG_DETAIL_RECORDER_FACTORY_H
#define PLATFORM_AAS_MW_LOG_DETAIL_RECORDER_FACTORY_H

#include "platform/aas/lib/os/fcntl_impl.h"
#include "platform/aas/mw/log/configuration/target_config_reader.h"
#include "platform/aas/mw/log/irecorder_factory.h"
#include "platform/aas/mw/log/recorder.h"

#include <memory>

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

class RecorderFactory : public IRecorderFactory
{
  public:
    RecorderFactory() noexcept = default;

    std::unique_ptr<Recorder> CreateFromConfiguration(
        amp::pmr::memory_resource* memory_resource) const noexcept override;
    std::unique_ptr<Recorder> CreateFromConfiguration(const std::unique_ptr<const ITargetConfigReader> config_reader,
                                                      amp::pmr::memory_resource* memory_resource) const noexcept;

    std::unique_ptr<Recorder> CreateWithConsoleLoggingOnly(
        amp::pmr::memory_resource* memory_resource) const noexcept override;

    std::unique_ptr<Recorder> CreateStub() const noexcept override;

    std::unique_ptr<Recorder> CreateRecorderFromLogMode(
        const LogMode& log_mode,
        const Configuration& config,
        amp::pmr::unique_ptr<bmw::os::Fcntl> fcntl_instance =
            bmw::os::FcntlImpl::Default(amp::pmr::get_default_resource()),
        amp::pmr::memory_resource* memory_resource = amp::pmr::get_default_resource()) const noexcept;

  private:
    static std::unique_ptr<Recorder> GetRemoteRecorder(const Configuration& config,
                                                       amp::pmr::memory_resource* memory_resource) noexcept;
    static std::unique_ptr<Recorder> GetFileRecorder(const Configuration& config,
                                                     amp::pmr::unique_ptr<bmw::os::Fcntl> fcntl_instance,
                                                     amp::pmr::memory_resource* memory_resource) noexcept;
    static std::unique_ptr<Recorder> GetConsoleRecorder(const Configuration& config,
                                                        amp::pmr::memory_resource* memory_resource) noexcept;
#ifdef __QNXNTO__
    static std::unique_ptr<Recorder> GetSystemRecorder(const Configuration& config,
                                                       amp::pmr::memory_resource* memory_resource) noexcept;
#endif
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  // PLATFORM_AAS_MW_LOG_DETAIL_RECORDER_FACTORY_H
