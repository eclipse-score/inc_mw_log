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


#include "platform/aas/mw/log/log_common.h"

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{
const PeriodSuffixMap PeriodToSuffix = {{typeid(std::atto::type), "as"},
                                        {typeid(std::femto::type), "fs"},
                                        {typeid(std::pico::type), "ps"},
                                        {typeid(std::nano::type), "ns"},
                                        {typeid(std::micro::type), "μs"},
                                        {typeid(std::milli::type), "ms"},
                                        {typeid(std::centi::type), "cs"},
                                        {typeid(std::deci::type), "ds"},
                                        {typeid(std::ratio<1>::type), "s"},
                                        {typeid(std::deca::type), "das"},
                                        {typeid(std::hecto::type), "hs"},
                                        {typeid(std::kilo::type), "ks"},
                                        {typeid(std::mega::type), "Ms"},
                                        {typeid(std::giga::type), "Gs"},
                                        {typeid(std::tera::type), "Ts"},
                                        {typeid(std::peta::type), "Ps"},
                                        {typeid(std::exa::type), "Es"},
                                        {typeid(std::ratio<60>::type), "min"},
                                        {typeid(std::ratio<3600>::type), "h"}};
}  // namespace detail

}  // namespace log
}  // namespace mw
}  // namespace bmw
