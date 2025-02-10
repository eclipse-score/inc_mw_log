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


#ifndef PLATFORM_AAS_MW_LOG_DETAIL_COMPOSITE_RECORDER_H
#define PLATFORM_AAS_MW_LOG_DETAIL_COMPOSITE_RECORDER_H

#include "platform/aas/mw/log/recorder.h"

#include <memory>
#include <vector>

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

/// \brief CompositeRecorder forwards the log statement to one or more concrete Recorder(s).
class CompositeRecorder final : public Recorder
{
  public:
    explicit CompositeRecorder(std::vector<std::unique_ptr<Recorder>> recorders) noexcept;

    amp::optional<SlotHandle> StartRecord(const amp::string_view context_id,
                                          const LogLevel log_level) noexcept override final;

    void StopRecord(const SlotHandle& composite_slot) noexcept override final;

    void Log(const SlotHandle& composite_slot, const bool arg) noexcept override final;
    void Log(const SlotHandle& composite_slot, const std::uint8_t arg) noexcept override final;
    void Log(const SlotHandle& composite_slot, const std::int8_t arg) noexcept override final;
    void Log(const SlotHandle& composite_slot, const std::uint16_t arg) noexcept override final;
    void Log(const SlotHandle& composite_slot, const std::int16_t arg) noexcept override final;
    void Log(const SlotHandle& composite_slot, const std::uint32_t arg) noexcept override final;
    void Log(const SlotHandle& composite_slot, const std::int32_t arg) noexcept override final;
    void Log(const SlotHandle& composite_slot, const std::uint64_t arg) noexcept override final;
    void Log(const SlotHandle& composite_slot, const std::int64_t arg) noexcept override final;
    void Log(const SlotHandle& composite_slot, const float arg) noexcept override final;
    void Log(const SlotHandle& composite_slot, const double arg) noexcept override final;
    void Log(const SlotHandle& composite_slot, const amp::string_view arg) noexcept override final;

    void Log(const SlotHandle& composite_slot, const LogHex8 arg) noexcept override final;
    void Log(const SlotHandle& composite_slot, const LogHex16 arg) noexcept override final;
    void Log(const SlotHandle& composite_slot, const LogHex32 arg) noexcept override final;
    void Log(const SlotHandle& composite_slot, const LogHex64 arg) noexcept override final;

    void Log(const SlotHandle& composite_slot, const LogBin8 arg) noexcept override final;
    void Log(const SlotHandle& composite_slot, const LogBin16 arg) noexcept override final;
    void Log(const SlotHandle& composite_slot, const LogBin32 arg) noexcept override final;
    void Log(const SlotHandle& composite_slot, const LogBin64 arg) noexcept override final;

    void Log(const SlotHandle& composite_slot, const LogRawBuffer arg) noexcept override final;

    void Log(const SlotHandle&, const LogSlog2Message) noexcept override final;

    const std::vector<std::unique_ptr<Recorder>>& GetRecorders() const noexcept;

    bool IsLogEnabled(const LogLevel&, const amp::string_view context) const noexcept override;

  private:
    std::vector<std::unique_ptr<Recorder>> recorders_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  // PLATFORM_AAS_MW_LOG_DETAIL_CONSOLE_RECORDER_H
