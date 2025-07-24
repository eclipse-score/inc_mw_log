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


#ifndef PLATFORM_AAS_MW_LOG_DETAIL_LOG_ENTRY_DESERIALIZE_H
#define PLATFORM_AAS_MW_LOG_DETAIL_LOG_ENTRY_DESERIALIZE_H

#include "platform/aas/mw/log/detail/logging_identifier.h"
#include "platform/aas/mw/log/log_level.h"
#include "visitor/visit_as_struct.h"

#include <amp_span.hpp>

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{
namespace log_entry_deserialization
{

//  SerializedVectorData is meant to be used as wrapper type to guide template overload resolution
struct SerializedVectorData
{
    amp::span<const uint8_t> data;
};

//  The type closely resemble bmw::mw::log::detail::LogEntry for all the member fields that are going to be deserialize
//  with the difference in payload which replaces std::vector with amp::span wrapped in custom structure i.e.
//  SerializedVectorData to allow template function overload to overwrite default deserialization behaviour
struct LogEntryDeserializationReflection
{
    bmw::mw::log::detail::LoggingIdentifier app_id{""};
    bmw::mw::log::detail::LoggingIdentifier ctx_id{""};
    SerializedVectorData serialized_vector_data;
    std::uint8_t num_of_args;
    bmw::mw::log::LogLevel log_level;

    amp::span<const std::uint8_t> GetPayload() const noexcept { return serialized_vector_data.data; }
};

STRUCT_TRACEABLE(LogEntryDeserializationReflection, app_id, ctx_id, serialized_vector_data, num_of_args, log_level)

}  // namespace log_entry_deserialization
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
#endif  // PLATFORM_AAS_MW_LOG_DETAIL_LOG_ENTRY_DESERIALIZE_H
