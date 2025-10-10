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


#include "platform/aas/mw/log/configuration/target_config_reader.h"

#include "amp_callback.hpp"
#include "platform/aas/lib/json/json_parser.h"
#include "platform/aas/lib/memory/split_string_view.h"
#include "platform/aas/mw/log/detail/error.h"
#include "platform/aas/mw/log/detail/initialization_reporter.h"

#include <amp_utility.hpp>

#include <functional>

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

namespace
{

constexpr StringLiteral kEcuIdKey{"ecuId"};
constexpr StringLiteral kAppIdKey{"appId"};
constexpr StringLiteral kAppDescriptionKey{"appDesc"};
constexpr StringLiteral kLogFilePathKey{"logFilePath"};
constexpr StringLiteral kLogModeKey{"logMode"};
constexpr StringLiteral kLogLevelKey{"logLevel"};
constexpr StringLiteral kLogLevelThresholdConsoleKey{"logLevelThresholdConsole"};
constexpr StringLiteral kContextConfigsKey{"contextConfigs"};
constexpr StringLiteral kContextNameKey{"name"};
constexpr StringLiteral kStackBufferSizeKey{"stackBufferSize"};
constexpr StringLiteral kRingBufferSizeKey{"ringBufferSize"};
constexpr StringLiteral kOverwriteOnFullKey{"overwriteOnFull"};
constexpr StringLiteral kNumberOfSlotsKey{"numberOfSlots"};
constexpr StringLiteral kSlotSizeBytes{"slotSizeBytes"};
constexpr StringLiteral kDatarouterUid{"datarouterUid"};
constexpr StringLiteral kDynamicDatarouterIdentifiers{"dynamicDatarouterIdentifiers"};

const std::unordered_map<amp::string_view, LogLevel> kStringToLogLevel{{{"kOff", LogLevel::kOff},
                                                                        {"kFatal", LogLevel::kFatal},
                                                                        {"kError", LogLevel::kError},
                                                                        {"kWarn", LogLevel::kWarn},
                                                                        {"kWarning", LogLevel::kWarn},
                                                                        {"kInfo", LogLevel::kInfo},
                                                                        {"kDebug", LogLevel::kDebug},
                                                                        {"kVerbose", LogLevel::kVerbose}}};

constexpr std::string::value_type kLogModeCombineChar = '|';

const std::unordered_map<amp::string_view, LogMode> kStringToLogMode{{{"kRemote", LogMode::kRemote},
                                                                      {"kConsole", LogMode::kConsole},
                                                                      {"kFile", LogMode::kFile},
                                                                      {"kSystem", LogMode::kSystem}}};

/// \brief Provide user feedback in case a configuration file contains errors.
template <typename T>
void ReportOnError(bmw::Result<T> result, const std::string& file_name) noexcept
{
    if (result.has_value() == true)
    {
        return;
    }

    ReportInitializationError(result.error(), amp::string_view{file_name.data(), file_name.size()});
}

// Forward declare GetElementAsImpl
template <typename ResultType, typename AsType = ResultType>
class GetElementAsImpl;

template <typename ResultType, typename AsType = ResultType>
auto GetElementAs(const bmw::json::Object& obj, const StringLiteral key) noexcept
{
    // To prevent using function template specializations, we use class template specialization in the implementation of
    // GetElementAs()
    return GetElementAsImpl<ResultType, AsType>::GetElementAs(obj, key);
}

template <typename ResultType, typename AsType>
class GetElementAsImpl
{
  public:
    static bmw::Result<ResultType> GetElementAs(const bmw::json::Object& obj, const StringLiteral key) noexcept
    {
        const auto find_result = obj.find(key);
        if (find_result == obj.end())
        {
            return bmw::MakeUnexpected(Error::kConfigurationOptionalJsonKeyNotFound, key);
        }

        return find_result->second.As<AsType>();
    }
};

template <typename T>
bmw::Result<std::reference_wrapper<const T>> GetElementAsRef(const bmw::json::Object& obj,
                                                             const StringLiteral key) noexcept
{
    return GetElementAs<std::reference_wrapper<const T>, T>(obj, key);
}

template <typename T>
using GetElementCallback = amp::callback<void(T)>;

template <typename T>
bmw::ResultBlank GetElementAndThen(const bmw::json::Object& obj,
                                   const StringLiteral key,
                                   GetElementCallback<T> update) noexcept
{
    const auto parser_result = GetElementAs<T>(obj, key);
    if (parser_result.has_value() == false)
    {
        
        return bmw::MakeUnexpected<bmw::Blank>(parser_result.error());
        
    }

    update(parser_result.value());

    return {};
}

bmw::ResultBlank ParseEcuId(const bmw::json::Object& root, Configuration& config) noexcept
{
    
    /* The lambda will be executed within this stack. Thus, all references are still valid */
    return GetElementAndThen<amp::string_view>(
        root, kEcuIdKey, [&config](auto value) noexcept { config.SetEcuId(value); });
    
}

bmw::ResultBlank ParseAppId(const bmw::json::Object& root, Configuration& config) noexcept
{
    
    /* The lambda will be executed within this stack. Thus, all references are still valid */
    return GetElementAndThen<amp::string_view>(
        root, kAppIdKey, [&config](auto value) noexcept { config.SetAppId(value); });
    
}

bmw::ResultBlank ParseAppDescription(const bmw::json::Object& root, Configuration& config) noexcept
{
    
    /* The lambda will be executed within this stack. Thus, all references are still valid */
    return GetElementAndThen<amp::string_view>(
        root, kAppDescriptionKey, [&config](auto value) noexcept { config.SetAppDescription(value); });
    
}

bmw::ResultBlank ParseLogFilePath(const bmw::json::Object& root, Configuration& config) noexcept
{
    
    /* The lambda will be executed within this stack. Thus, all references are still valid */
    return GetElementAndThen<amp::string_view>(
        root, kLogFilePathKey, [&config](auto value) noexcept { config.SetLogFilePath(value); });
    
}

/// \brief Returns the corresponding log mode of the string.
bmw::Result<LogMode> LogModeFromString(const amp::string_view str) noexcept
{
    const auto result = kStringToLogMode.find(str);

    if (result == kStringToLogMode.end())
    {
        return MakeUnexpected(Error::kInvalidLogModeString, "Expected `kRemote`, `kConsole`, `kSystem` or `kFile`.");
    }

    return result->second;
}

/// \brief Returns the corresponding combined log mode(s) of the string.
bmw::Result<std::unordered_set<LogMode>> LogModesFromString(const amp::string_view str) noexcept
{
    const auto segments = bmw::memory::LazySplitStringView{str, kLogModeCombineChar};

    std::unordered_set<LogMode> result;

    for (const amp::string_view segment : segments)
    {
        const auto log_mode = LogModeFromString(segment);
        if (log_mode.has_value() == false)
        {
            return MakeUnexpected(Error::kInvalidLogModeString,
                                  "Expected `kRemote`, `kConsole`, `kSystem` or `kFile`.");
        }
        amp::ignore = result.emplace(log_mode.value());
    }
    return result;
}

bmw::ResultBlank ParseLogMode(const bmw::json::Object& root, Configuration& config) noexcept
{
    
    /* The lambda will be executed within this stack. Thus, all references are still valid */
    return GetElementAndThen<amp::string_view>(root, kLogModeKey, [&config](auto value) noexcept {
        const auto log_mode_result = LogModesFromString(value);
        if (log_mode_result.has_value())
        {
            config.SetLogMode(log_mode_result.value());  // LCOV_EXCL_BR_LINE: no branches here to be covered.
        }
    });
    
}

/// \brief Returns the corresponding log level of the string.
bmw::Result<LogLevel> LogLevelFromString(const amp::string_view str) noexcept
{
    const auto result = kStringToLogLevel.find(str);
    if (result == kStringToLogLevel.end())
    {
        return MakeUnexpected(Error::kInvalidLogLevelString,
                              "Expected `kOff`, `kFatal`, `kWarn`, `kError`, `kInfo`, `kDebug` or `kVerbose`.");
    }
    return result->second;
}

/// \brief Returns the element of a JSON object as a LogLevel.
template <>
class GetElementAsImpl<LogLevel>
{
  public:
    static bmw::Result<LogLevel> GetElementAs(const bmw::json::Object& obj, const StringLiteral key) noexcept
    {
        const auto string_result = GetElementAsImpl<amp::string_view>::GetElementAs(obj, key);
        if (string_result.has_value() == false)
        {
            
            return bmw::MakeUnexpected<LogLevel>(string_result.error());
            
        }

        return LogLevelFromString(string_result.value());
    }
};

bmw::ResultBlank ParseLogLevel(const bmw::json::Object& root, Configuration& config) noexcept
{
    
    /* The lambda will be executed within this stack. Thus, all references are still valid */
    return GetElementAndThen<amp::string_view>(root, kLogLevelKey, [&config](auto value) noexcept {
        const auto log_level_result = LogLevelFromString(value);
        if (log_level_result.has_value())
        {
            config.SetDefaultLogLevel(log_level_result.value());  // LCOV_EXCL_BR_LINE: no branches here to be covered.
        }
    });
    
}

bmw::ResultBlank ParseLogLevelConsole(const bmw::json::Object& root, Configuration& config) noexcept
{
    
    /* The lambda will be executed within this stack. Thus, all references are still valid */
    return GetElementAndThen<amp::string_view>(root, kLogLevelThresholdConsoleKey, [&config](auto value) noexcept {
        const auto log_level_result = LogLevelFromString(value);
        if (log_level_result.has_value())
        {
            config.SetDefaultConsoleLogLevel(log_level_result.value());
        }
    });
    
}

bmw::ResultBlank ParseContextLogLevel(const bmw::json::Object& root,
                                      Configuration& config,
                                      const std::string& path_for_reporting) noexcept
{
    const auto context_config = GetElementAsRef<bmw::json::List>(root, kContextConfigsKey);
    if (context_config.has_value() == false)
    {
        //  coverage: coverage shown by manual inspection.
        //  Reasoning: building without code optimization (-O0) it is possible to set and hit the breakpoint.
        //  See also: 
        
        return bmw::MakeUnexpected<bmw::Blank>(context_config.error());
        
    }

    auto context_config_map = config.GetContextLogLevel();

    
    for (const auto& context_item : context_config.value().get())
    {
        const auto context_result = context_item.As<bmw::json::Object>();
        if (context_result.has_value() == false)
        {
            ReportOnError(context_result, path_for_reporting);
            continue;
        }
        const auto context_obj = context_result.value();

        const auto context_name_result = GetElementAsRef<std::string>(context_obj, kContextNameKey);
        if (context_name_result.has_value() == false)
        {
            ReportOnError(context_name_result, path_for_reporting);
            continue;
        }

        const auto context_log_level_result = GetElementAs<LogLevel>(context_obj, kLogLevelKey);
        if (context_log_level_result.has_value() == false)
        {
            ReportOnError(context_name_result, path_for_reporting);
            continue;
        }

        context_config_map[LoggingIdentifier{context_name_result.value().get()}] = context_log_level_result.value();
    }
    
    config.SetContextLogLevel(context_config_map);

    return {};
}

bmw::ResultBlank ParseStackBufferSize(const bmw::json::Object& root, Configuration& config) noexcept
{
    
    /* The lambda will be executed within this stack. Thus, all references are still valid */
    return GetElementAndThen<std::size_t>(
        root, kStackBufferSizeKey, [&config](auto value) noexcept { config.SetStackBufferSize(value); });
    
}

bmw::ResultBlank ParseRingBufferSize(const bmw::json::Object& root, Configuration& config) noexcept
{
    
    /* The lambda will be executed within this stack. Thus, all references are still valid */
    return GetElementAndThen<std::size_t>(
        root, kRingBufferSizeKey, [&config](auto value) noexcept { config.SetRingBufferSize(value); });
    
}

bmw::ResultBlank ParseOverwriteOnFull(const bmw::json::Object& root, Configuration& config) noexcept
{
    
    /* The lambda will be executed within this stack. Thus, all references are still valid */
    return GetElementAndThen<bool>(
        root, kOverwriteOnFullKey, [&config](auto value) noexcept { config.SetRingBufferOverwriteOnFull(value); });
    
}

bmw::ResultBlank ParseNumberOfSlots(const bmw::json::Object& root, Configuration& config) noexcept
{
    
    /* The lambda will be executed within this stack. Thus, all references are still valid */
    return GetElementAndThen<std::size_t>(
        root, kNumberOfSlotsKey, [&config](auto value) noexcept { config.SetNumberOfSlots(value); });
    
}

bmw::ResultBlank ParseSlotSizeBytes(const bmw::json::Object& root, Configuration& config) noexcept
{
    
    /* The lambda will be executed within this stack. Thus, all references are still valid */
    return GetElementAndThen<std::size_t>(
        root, kSlotSizeBytes, [&config](auto value) noexcept { config.SetSlotSizeInBytes(value); });
    
}

bmw::ResultBlank ParseDatarouterUid(const bmw::json::Object& root, Configuration& config) noexcept
{
    
    /* The lambda will be executed within this stack. Thus, all references are still valid */
    return GetElementAndThen<std::size_t>(
        root, kDatarouterUid, [&config](const auto value) noexcept { config.SetDataRouterUid(value); });
    
}

bmw::ResultBlank ParseDynamicDatarouterIdentifiers(const bmw::json::Object& root, Configuration& config) noexcept
{
    
    /* The lambda will be executed within this stack. Thus, all references are still valid */
    return GetElementAndThen<std::size_t>(root, kDynamicDatarouterIdentifiers, [&config](const auto value) noexcept {
        config.SetDynamicDatarouterIdentifiers(value);
    });
    
}

void ParseConfigurationElements(const bmw::json::Object& root, const std::string& path, Configuration& config) noexcept
{
    ReportOnError(ParseEcuId(root, config), path);
    ReportOnError(ParseAppId(root, config), path);
    ReportOnError(ParseAppDescription(root, config), path);
    ReportOnError(ParseLogFilePath(root, config), path);
    ReportOnError(ParseLogMode(root, config), path);
    ReportOnError(ParseLogLevel(root, config), path);
    ReportOnError(ParseLogLevelConsole(root, config), path);
    ReportOnError(ParseContextLogLevel(root, config, path), path);
    ReportOnError(ParseStackBufferSize(root, config), path);
    ReportOnError(ParseRingBufferSize(root, config), path);
    ReportOnError(ParseOverwriteOnFull(root, config), path);
    ReportOnError(ParseNumberOfSlots(root, config), path);
    ReportOnError(ParseSlotSizeBytes(root, config), path);
    ReportOnError(ParseDatarouterUid(root, config), path);
    ReportOnError(ParseDynamicDatarouterIdentifiers(root, config), path);
}

bmw::Result<Configuration> ParseAndUpdateConfiguration(const std::string& path, Configuration config) noexcept
{
    const bmw::json::JsonParser json_parser_obj;
    // FromFile() is safe, if the JSON file is stored in stored on qtsafefs (integrity protection).
    // See 
    // NOLINTNEXTLINE(bmw-banned-function) - Argumentation is above.
    const auto json_result = json_parser_obj.FromFile(path);
    if (json_result.has_value() == false)
    {
        
        return bmw::MakeUnexpected<Configuration>(json_result.error());
        
    }

    const auto root_result = json_result.value().As<bmw::json::Object>();
    if (root_result.has_value() == false)
    {
        //  coverage: coverage shown by manual inspection.
        //  Reasoning: building without code optimization (-O0) it is possible to set and hit the breakpoint.
        //  See also: 
        
        return bmw::MakeUnexpected<Configuration>(root_result.error());
        
    }

    ParseConfigurationElements(root_result->get(), path, config);
    
    return config;
    
}

}  // namespace



TargetConfigReader::TargetConfigReader(std::unique_ptr<IConfigurationFileDiscoverer> discoverer) noexcept
    : ITargetConfigReader{}, discoverer_(std::move(discoverer))
{
}




bmw::Result<Configuration> TargetConfigReader::ReadConfig() const noexcept
{
    const auto config_files = discoverer_->FindConfigurationFiles();

    if (config_files.empty())
    {
        return MakeUnexpected(Error::kConfigurationFilesNotFound);
    }

    Configuration config{};

    // Update the config instance by iterating over the config_files
    // Each config file can overwrite the previous config files.
    
    for (const auto& config_file : config_files)
    {
        auto result = ParseAndUpdateConfiguration(config_file, config);

        if (result.has_value() == false)
        {
            ReportOnError(result, config_file);
            continue;
        }

        config = std::move(result.value());
    }
    

    
    return config;
    
}


}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
