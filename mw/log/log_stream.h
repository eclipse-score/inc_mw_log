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


#ifndef PLATFORM_AAS_MW_LOG_LOG_STREAM_H
#define PLATFORM_AAS_MW_LOG_LOG_STREAM_H

// Be careful what you include here. Each additional header will be included in logging.h and thus exposed to the user.
// We need to try to keep the includes low to reduce the compile footprint of using this library.
#include "platform/aas/mw/log/detail/logging_identifier.h"
#include "platform/aas/mw/log/log_common.h"
#include "platform/aas/mw/log/log_level.h"
#include "platform/aas/mw/log/log_types.h"
#include "platform/aas/mw/log/slot_handle.h"

#include "amp_optional.hpp"
#include "amp_string_view.hpp"

#include <type_traits>

namespace bmw
{
namespace mw
{
namespace log
{

// Forward declaration.
// 
class Recorder;

template <typename T>
constexpr static bool IsCharPtrType() noexcept
{
    using PointerType = std::remove_reference_t<T>;
    if constexpr (std::is_pointer_v<PointerType>)
    {
        using ValueType = std::remove_pointer_t<PointerType>;
        return std::is_same_v<LogString::CharType, std::remove_const_t<ValueType>>;
    }
    return false;
}

template <typename T>
constexpr static bool IsCharArrayType() noexcept
{
    using ArrayType = amp::remove_cvref_t<T>;
    return std::is_array_v<ArrayType> && std::is_same_v<LogString::CharType, std::remove_extent_t<ArrayType>>;
}

template <typename T>
constexpr static bool IsLogRawBufferType() noexcept
{
    return std::is_same_v<LogRawBuffer, amp::remove_cvref_t<T>>;
}

namespace detail
{
class LogStreamFactory;

template <typename... Types>
struct TypesHolder
{
};

template <typename T, typename... SupportedTypes>
constexpr static bool LogStreamSupports(TypesHolder<SupportedTypes...> /*unused*/) noexcept
{
    if constexpr (not(IsCharPtrType<T>()))
    {
        using RequestedType = amp::remove_cvref_t<T>;
        return (std::is_convertible_v<RequestedType, SupportedTypes> || ...);
    }
    else
    {
        return false;
    }
}
}  // namespace detail

template <typename T>
constexpr static bool LogStreamSupports() noexcept
{
    using SupportedTypes = detail::TypesHolder<bool,
                                               float,
                                               double,
                                               std::int8_t,
                                               std::int16_t,
                                               std::int32_t,
                                               std::int64_t,
                                               std::uint8_t,
                                               std::uint16_t,
                                               std::uint32_t,
                                               std::uint64_t,
                                               LogBin8,
                                               LogBin16,
                                               LogBin32,
                                               LogBin64,
                                               LogHex8,
                                               LogHex16,
                                               LogHex32,
                                               LogHex64,
                                               LogString,
                                               LogRawBuffer,
                                               LogSlog2Message>;
    return detail::LogStreamSupports<T>(SupportedTypes{});
}

// \brief User-Facing RAII class that manages a LogStream and can be used to log data. The log message will only be
// flushed upon destruction of this type. It is not possible to reuse one LogStream for multiple different log
// messages. Logging is a best effort operation, if it is not possible to log a message due to some reason, this class
// will not forward the respective messages.
//
// \details This class only supports the logging of the following basic data types:
//          * bool
//          * float
//          * double
//          * std::int8_t
//          * std::int16_t
//          * std::int32_t
//          * std::int64_t
//          * std::uint8_t
//          * std::uint16_t
//          * std::uint32_t
//          * std::uint64_t
//          * mw::log::LogBin8
//          * mw::log::LogBin16
//          * mw::log::LogBin32
//          * mw::log::LogBin64
//          * mw::log::LogHex8
//          * mw::log::LogHex16
//          * mw::log::LogHex32
//          * mw::log::LogHex64
//          * mw::log::LogString
//          * mw::log::LogRawBuffer
//          * std::string (via implicit conversion to mw::log::LogString)
//          * std::string_view (via implicit conversion to mw::log::LogString)
//          * amp::string_view (via implicit conversion to mw::log::LogString)
//          * amp::pmr::string (via implicit conversion to mw::log::LogString)
//          * ara::core::StringView (via implicit conversion to mw::log::LogString)
//          * std::array<[const] char> (via implicit conversion to mw::log::LogString)
//          * pointer to const char (deprecated)
//          * C-style array of char
//          * custom data struct LogSlog2Message, see struct description
//
//          If a user wants to log a custom data type, he needs to extend the possibility as follows in the context of
//          his data type:
//
//          bmw::mw::LogStream& operator<<(bmw::mw::LogStream& stream, const YourAwesomeType& type)
//          {
//             stream << YourAwesomeType.getSomething(); // custom logic on how to represent your type as Log-Message
//             return stream;
//          }

/* False positive: LogStream shouldn't be considered as const. It's mandated by the ara::log API. */
/// \public
class LogStream final
{
  public:
    /// \brief Stream operator which enables logging of supported data types.
    /// \public
    /// \note Only the types mentioned in class description are supported by default.
    /// \details Similar to std::cout it is not safe to access this stream from multiple threads!
    template <typename T>
    std::enable_if_t<LogStreamSupports<T>(), LogStream&> operator<<(const T& value) & noexcept
    {
        return Log(value);
    }

    /// \brief Deprecated stream operator which enables logging of C-style string via character pointer.
    /// \note The pointed-to character sequence must be null-terminated!
    /// \details Similar to std::cout it is not safe to access this stream from multiple threads!
    template <typename T>
    [[deprecated(
        "SPP_DEPRECATION(performance): "
        "Logging a plain character pointer is discouraged for performance reasons since that requires a length "
        "determination of the pointed-to character sequence at runtime and results in a sequential memory scan. "
        "Instead, since almost all string-like types are implicitly nothrow-convertible to `mw::log::LogString`, "
        "these should be logged as-is and not via their underlying character pointer! If that's not possible, a "
        "custom overload of `operator<<` for the particular user-defined type should be provided.")]] std::
        enable_if_t<IsCharPtrType<T>(), LogStream&>
        operator<<(T char_ptr) &
    {
        if (char_ptr != nullptr)
        {
            const LogString value{char_ptr, LogString::TraitsType::length(char_ptr)};
            return Log(value);
        }
        return *this;
    }

    /// \brief Stream operator which enables logging of a string literal (-> character array).
    /// \public
    /// \details Similar to std::cout it is not safe to access this stream from multiple threads!
    template <std::size_t N>
    // NOLINTNEXTLINE(modernize-avoid-c-arrays): required for logging string literals without array-to-pointer decay
    LogStream& operator<<(const LogString::CharType (&array)[N]) noexcept
    {
        const LogString value{std::forward<decltype(array)>(array)};
        return Log(value);
    }

    // \brief LogStream is move-construction only
    LogStream(const LogStream&) noexcept = delete;
    LogStream(LogStream&&) noexcept;
    LogStream& operator=(const LogStream&) noexcept = delete;
    LogStream& operator=(LogStream&&) noexcept = delete;
    ~LogStream() noexcept;

    /// \brief Flushes the current buffer and prepares a new one.
    /// \public
    /// \details Calling mw::log::LogStream::Flush is only necessary if the mw::log::LogStream
    /// object is going to be re-used within the same scope. Otherwise, if the
    /// object goes out of scope (e.g. end of function block) then the flushing operation will
    /// be done internally by the destructor. It is important to note that the mw::log::-
    /// LogStream::Flush command does not empty the buffer, but it forwards the buffer’s
    /// current contents to the Logging framework.
    void Flush() noexcept;

    
  private:
    

    // We can't make the ctor public, the ctor intended to be private to avoid instance instantiation by the user
    // but it is needed internally by LogStreamFactory
    // 
    friend bmw::mw::log::detail::LogStreamFactory;

    LogStream(Recorder&, Recorder&, const LogLevel, const amp::string_view) noexcept;

    /// \brief Internal methods that perform logging of supported data types.
    LogStream& Log(const bool) noexcept;
    LogStream& Log(const float) noexcept;
    LogStream& Log(const double) noexcept;
    LogStream& Log(const LogString) noexcept;
    LogStream& Log(const std::int8_t) noexcept;
    LogStream& Log(const std::int16_t) noexcept;
    LogStream& Log(const std::int32_t) noexcept;
    LogStream& Log(const std::int64_t) noexcept;
    LogStream& Log(const std::uint8_t) noexcept;
    LogStream& Log(const std::uint16_t) noexcept;
    LogStream& Log(const std::uint32_t) noexcept;
    LogStream& Log(const std::uint64_t) noexcept;
    LogStream& Log(const LogBin8& value) noexcept;
    LogStream& Log(const LogBin16& value) noexcept;
    LogStream& Log(const LogBin32& value) noexcept;
    LogStream& Log(const LogBin64& value) noexcept;
    LogStream& Log(const LogHex8& value) noexcept;
    LogStream& Log(const LogHex16& value) noexcept;
    LogStream& Log(const LogHex32& value) noexcept;
    LogStream& Log(const LogHex64& value) noexcept;
    LogStream& Log(const LogSlog2Message& value) noexcept;

    /// \note Log method for `LogRawBuffer` must be templated so that the overload for `LogString` always
    ///       has higher precedence during overload resolution in case the parameter type is string-like.
    template <typename RawBufferType>
    std::enable_if_t<IsLogRawBufferType<RawBufferType>(), LogStream&> Log(const RawBufferType&) noexcept;

    /// \brief Internal method which forwards the logging operation to `LogStream`'s provided recorder(s).
    template <typename T>
    LogStream& LogWithRecorder(const T value) noexcept;

    template <typename ReturnValue, typename... ArgsOfFunction, typename... ArgsPassed>
    // Checker overly strict, pointer of object not null and only existing functions called (ensure by typeset)
    // NOLINTNEXTLINE(bmw-no-pointer-to-member): See above
    ReturnValue CallOnRecorder(ReturnValue (Recorder::*arbitrary_function)(ArgsOfFunction...) noexcept,
                               ArgsPassed&&... args) noexcept;

    
    Recorder& recorder_;
    Recorder& fallback_recorder_;
    amp::optional<SlotHandle> slot_;

    detail::LoggingIdentifier context_id_;
    LogLevel log_level_;
    
};


/// \brief Stream operator overload which enables logging to a `LogStream` rvalue.
/// \public
template <typename T>

std::enable_if_t<not(IsCharArrayType<T>()), LogStream&> operator<<(LogStream&& out, T&& value) noexcept
{
    return (out << std::forward<T>(value));
}

/// @brief Writes chrono duration parameter as text into message.
/// \public
template <typename Rep, typename Period>
LogStream& operator<<(LogStream& out, const std::chrono::duration<Rep, Period>& value) noexcept
{
    return (out << value.count() << DurationUnitSuffix<Period>());
}

/// \brief Support for signed long long (transforms into int64_t on 64 bit platforms)
/// \public
template <
    typename T = void,
    typename std::enable_if<((!std::is_same<long long, int64_t>::value) && (sizeof(long long) == sizeof(int64_t))),
                            T>::type* = nullptr>
inline LogStream& operator<<(LogStream& out, long long value) noexcept
{
    return (out << static_cast<int64_t>(value));
}

// Non-standard extensions
// To use these utilities please import the namespace, like this for example:
// `using ara::log::bmw_ext::operator<<;`
namespace bmw_ext
{

template <typename EnumerationT, std::enable_if_t<std::is_enum<EnumerationT>::value, bool> = true>


/// \public
LogStream& operator<<(LogStream& out, EnumerationT enumvalue)

{
    return (out << +typename std::underlying_type<EnumerationT>::type(enumvalue));
}
}  // namespace bmw_ext

}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  // PLATFORM_AAS_MW_LOG_LOG_STREAM_H
