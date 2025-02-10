// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#include "nvconfig.h"

#include "platform/aas/lib/json/json_parser.h"

#include "amp_string_view.hpp"

namespace bmw
{
namespace mw
{
namespace log
{

namespace
{
constexpr mw::log::LogLevel kDefaultLogLevel = mw::log::LogLevel::kInfo;

config::NvMsgDescriptor GetMsgDescriptor(const std::uint32_t id,
                                         const bmw::json::Object& object_array_value,
                                         const bmw::json::Object::const_iterator& object_appid_iterator,
                                         const bmw::json::Object::const_iterator& object_ctxid_iterator) noexcept
{
    const auto& object_loglevel_iterator = object_array_value.find("loglevel");
    
    /* log_level visibility is correct, since it is used in the NvMsgDescriptor. */
    auto log_level = kDefaultLogLevel;
    
    if (object_loglevel_iterator != object_array_value.end())
    {
        log_level = mw::log::TryGetLogLevelFromU8(object_loglevel_iterator->second.As<std::uint8_t>().value())
                        .value_or(kDefaultLogLevel);
    }

    auto object_appid_result = object_appid_iterator->second.As<amp::string_view>();
    
    /* Braces are correctly placed. */
    const bmw::mw::log::detail::LoggingIdentifier appid(object_appid_result.value());
    
    auto object_ctxid_result = object_ctxid_iterator->second.As<amp::string_view>();
    
    /* Braces are correctly placed. */
    const bmw::mw::log::detail::LoggingIdentifier ctxid(object_ctxid_result.value());
    
    
    /* It's initialization list, not variables declaring. */
    
    return config::NvMsgDescriptor{id, appid, ctxid, log_level};
    
    
}

NvConfig::ReadResult HandleParseResult(const bmw::json::Object& parse_result, NvConfig::typemap_t& typemap) noexcept
{
    for (auto& result_iterator : parse_result)
    {
        auto object_array_result = result_iterator.second.As<bmw::json::Object>();
        
        if (!object_array_result.has_value())
        
        {
            return NvConfig::ReadResult::kERROR_PARSE;
        }

        auto& object_array_value = object_array_result.value().get();
        const auto& object_ctxid_iterator = object_array_value.find("ctxid");
        if (object_ctxid_iterator == object_array_value.end())
        {
            return NvConfig::ReadResult::kERROR_CONTENT;
        }

        const auto& object_id_iterator = object_array_value.find("id");
        if (object_id_iterator == object_array_value.end())
        {
            return NvConfig::ReadResult::kERROR_CONTENT;
        }

        const auto& object_appid_iterator = object_array_value.find("appid");
        if (object_appid_iterator == object_array_value.end())
        {
            return NvConfig::ReadResult::kERROR_CONTENT;
        }

        auto id = object_id_iterator->second.As<std::uint32_t>();
        
        
        if (id.has_value() == false)
        
        
        {
            return NvConfig::ReadResult::kERROR_CONTENT;
        }

        auto object_name = result_iterator.first.GetAsStringView();
        typemap[object_name.data()] =
            GetMsgDescriptor(id.value(), object_array_value, object_appid_iterator, object_ctxid_iterator);
    }
    return NvConfig::ReadResult::kOK;
}

}  // namespace


NvConfig::NvConfig(const std::string& file_path) : json_path_(file_path), typemap_{} {}


NvConfig::ReadResult NvConfig::parseFromJson() noexcept


{
    const bmw::json::JsonParser json_parser_obj;
    // FromFile() is safe, if the JSON file is stored in stored on qtsafefs (integrity protection).
    // See 
    // NOLINTNEXTLINE(bmw-banned-function) - Argumentation is above.
    auto root = json_parser_obj.FromFile(json_path_);
    
    
    if (!root.has_value())
    
    
    {
        return ReadResult::kERROR_PARSE;
    }

    auto parse_result = root.value().As<bmw::json::Object>();
    
    if (!parse_result.has_value())
    
    {
        return ReadResult::kERROR_PARSE;
    }

    return HandleParseResult(parse_result.value(), typemap_);
}




const config::NvMsgDescriptor* NvConfig::getDltMsgDesc(const std::string& typeName) const noexcept



{
    auto desc = typemap_.find(typeName);

    if (desc != typemap_.end())
    {
        return &desc->second;
    }
    else
    {
        return nullptr;
    }
}

}  // namespace log
}  // namespace mw
}  // namespace bmw
