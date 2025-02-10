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


#ifndef PLATFORM_AAS_MW_LOG_DETAIL_DLT_FORMAT_H
#define PLATFORM_AAS_MW_LOG_DETAIL_DLT_FORMAT_H

#include "platform/aas/mw/log/detail/add_argument_result.h"
#include "platform/aas/mw/log/detail/integer_representation.h"
#include "platform/aas/mw/log/detail/verbose_payload.h"
#include "platform/aas/mw/log/log_types.h"

#include "amp_string_view.hpp"

#include <cstdint>

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

class DLTFormat
{
  public:
    static AddArgumentResult Log(VerbosePayload&, const bool) noexcept;
    static AddArgumentResult Log(VerbosePayload&,
                                 const std::uint8_t,
                                 const IntegerRepresentation = IntegerRepresentation::kDecimal) noexcept;
    static AddArgumentResult Log(VerbosePayload&,
                                 const std::uint16_t,
                                 const IntegerRepresentation = IntegerRepresentation::kDecimal) noexcept;
    static AddArgumentResult Log(VerbosePayload&,
                                 const std::uint32_t,
                                 const IntegerRepresentation = IntegerRepresentation::kDecimal) noexcept;
    static AddArgumentResult Log(VerbosePayload&,
                                 const std::uint64_t,
                                 const IntegerRepresentation = IntegerRepresentation::kDecimal) noexcept;
    static AddArgumentResult Log(VerbosePayload&,
                                 const std::int8_t,
                                 const IntegerRepresentation = IntegerRepresentation::kDecimal) noexcept;
    static AddArgumentResult Log(VerbosePayload&,
                                 const std::int16_t,
                                 const IntegerRepresentation = IntegerRepresentation::kDecimal) noexcept;
    static AddArgumentResult Log(VerbosePayload&,
                                 const std::int32_t,
                                 const IntegerRepresentation = IntegerRepresentation::kDecimal) noexcept;
    static AddArgumentResult Log(VerbosePayload&,
                                 const std::int64_t,
                                 const IntegerRepresentation = IntegerRepresentation::kDecimal) noexcept;
    static AddArgumentResult Log(VerbosePayload&,
                                 const LogHex8,
                                 const IntegerRepresentation = IntegerRepresentation::kHex) noexcept;
    static AddArgumentResult Log(VerbosePayload&,
                                 const LogHex16,
                                 const IntegerRepresentation = IntegerRepresentation::kHex) noexcept;
    static AddArgumentResult Log(VerbosePayload&,
                                 const LogHex32,
                                 const IntegerRepresentation = IntegerRepresentation::kHex) noexcept;
    static AddArgumentResult Log(VerbosePayload&,
                                 const LogHex64,
                                 const IntegerRepresentation = IntegerRepresentation::kHex) noexcept;
    static AddArgumentResult Log(VerbosePayload&,
                                 const LogBin8,
                                 const IntegerRepresentation = IntegerRepresentation::kBinary) noexcept;
    static AddArgumentResult Log(VerbosePayload&,
                                 const LogBin16,
                                 const IntegerRepresentation = IntegerRepresentation::kBinary) noexcept;
    static AddArgumentResult Log(VerbosePayload&,
                                 const LogBin32,
                                 const IntegerRepresentation = IntegerRepresentation::kBinary) noexcept;
    static AddArgumentResult Log(VerbosePayload&,
                                 const LogBin64,
                                 const IntegerRepresentation = IntegerRepresentation::kBinary) noexcept;
    static AddArgumentResult Log(VerbosePayload&, const float) noexcept;
    static AddArgumentResult Log(VerbosePayload&, const double) noexcept;
    static AddArgumentResult Log(VerbosePayload&, const amp::string_view) noexcept;
    static AddArgumentResult Log(VerbosePayload&, const LogRawBuffer) noexcept;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  // PLATFORM_AAS_MW_LOG_DETAIL_DLT_FORMAT_H
