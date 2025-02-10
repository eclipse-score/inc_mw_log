// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#ifndef PLATFORM_AAS_MW_LOG_DETAIL_SLOG_SLOG_BACKEND_H
#define PLATFORM_AAS_MW_LOG_DETAIL_SLOG_SLOG_BACKEND_H

#include "platform/aas/lib/os/qnx/slog2_impl.h"
#include "platform/aas/mw/log/detail/backend.h"
#include "platform/aas/mw/log/detail/circular_allocator.h"

#include <amp_string_view.hpp>
#include <cstdint>

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

class SlogBackend final : public Backend
{
  public:
    explicit SlogBackend(const std::size_t number_of_slots,
                         const LogRecord& initial_slot_value,
                         const amp::string_view app_id,
                         amp::pmr::unique_ptr<bmw::os::qnx::Slog2> slog2_instance) noexcept;

    amp::optional<SlotHandle> ReserveSlot() noexcept override;
    void FlushSlot(const SlotHandle& slot) noexcept override;
    LogRecord& GetLogRecord(const SlotHandle& slot) noexcept override;

  private:
    void Init(const std::uint8_t verbosity) noexcept;

    std::string app_id_;
    CircularAllocator<bmw::mw::log::detail::LogRecord> buffer_;
    slog2_buffer_t slog_buffer_;
    slog2_buffer_set_config_t slog_buffer_config_;
    amp::pmr::unique_ptr<bmw::os::qnx::Slog2> slog2_instance_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  // PLATFORM_AAS_MW_LOG_DETAIL_SLOG_SLOG_BACKEND_H
