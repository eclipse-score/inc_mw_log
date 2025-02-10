// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#ifndef PLATFORM_AAS_MW_LOG_LOGGER_CONTAINER_H
#define PLATFORM_AAS_MW_LOG_LOGGER_CONTAINER_H

#include "platform/aas/mw/log/detail/wait_free_stack/wait_free_stack.h"
#include "platform/aas/mw/log/logger.h"
#include "platform/aas/mw/log/slot_handle.h"
#include <amp_static_vector.hpp>

namespace bmw
{
namespace mw
{
namespace log
{






class LoggerContainer final 




{
  public:
    explicit LoggerContainer();

    Logger& GetLogger(
        const amp::string_view context) noexcept; 

    size_t GetCapacity() const noexcept;

    Logger& GetDefaultLogger() noexcept;

    
  private:
    
    Logger& InsertNewLogger(const amp::string_view context) noexcept;

    amp::optional<std::reference_wrapper<Logger>> FindExistingLogger(const amp::string_view context) noexcept;
    
    detail::WaitFreeStack<Logger> stack_;
    Logger default_logger_;
    
};




}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  // PLATFORM_AAS_MW_LOG_LOGGER_CONTAINER_H
