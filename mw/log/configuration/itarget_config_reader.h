// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#ifndef PLATFORM_AAS_MW_LOG_DETAIL_ITARGET_CONFIG_READER_H
#define PLATFORM_AAS_MW_LOG_DETAIL_ITARGET_CONFIG_READER_H

#include "platform/aas/lib/result/result.h"
#include "platform/aas/mw/log/configuration/configuration.h"

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

class ITargetConfigReader
{
  public:
    ITargetConfigReader() noexcept = default;
    ITargetConfigReader(ITargetConfigReader&&) noexcept = delete;
    ITargetConfigReader(const ITargetConfigReader&) noexcept = delete;
    ITargetConfigReader& operator=(ITargetConfigReader&&) noexcept = delete;
    ITargetConfigReader& operator=(const ITargetConfigReader&) noexcept = delete;
    virtual ~ITargetConfigReader() noexcept;

    virtual bmw::Result<Configuration> ReadConfig() const noexcept = 0;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  // PLATFORM_AAS_MW_LOG_DETAIL_ITARGET_CONFIG_READER_H
