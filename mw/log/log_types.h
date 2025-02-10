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



#ifndef PLATFORM_AAS_MW_LOG_TYPES_H
#define PLATFORM_AAS_MW_LOG_TYPES_H

#include <amp_assert.hpp>
#include <amp_span.hpp>
#include <amp_string_view.hpp>

#include <cstdint>
#include <functional>
#include <iterator>
#include <type_traits>
#include <vector>

namespace bmw
{
namespace mw
{
namespace log
{

/// \brief Helper type to log an uint8 in hexadecimal representation.
/// \public
struct LogHex8
{
    std::uint8_t value;
};

/// \brief Helper type to log an uint16 in hexadecimal representation.
/// \public
struct LogHex16
{
    std::uint16_t value;
};

/// \brief Helper type to log an uint32 in hexadecimal representation.
/// \public
struct LogHex32
{
    std::uint32_t value;
};

/// \brief Helper type to log an uint64 in hexadecimal representation.
/// \public
struct LogHex64
{
    std::uint64_t value;
};

/// \brief Helper type to log an uint8 in binary representation.
/// \public
struct LogBin8
{
    std::uint8_t value;
};

/// \brief Helper type to log an uint16 in binary representation.
/// \public
struct LogBin16
{
    std::uint16_t value;
};

/// \brief Helper type to log an uint32 in binary representation.
/// \public
struct LogBin32
{
    std::uint32_t value;
};

/// \brief Helper type to log an uint64 in binary representation.
/// \public
struct LogBin64
{
    std::uint64_t value;
};

namespace detail
{
/// @brief Used to obtain the iterator type of the type `Range`.
template <typename Range>
using IteratorType = decltype(std::begin(std::declval<Range&>()));

template <typename Iterator, typename R = decltype(*std::declval<Iterator&>()), typename = R&>
using IteratorReferenceHelper = R;

/// @brief Determines the reference type of `Iterator`.
template <typename Iterator>
using IteratorReferenceType = IteratorReferenceHelper<Iterator>;

/// @brief Determines whether type `RangeType` is a range of `ElementType` and not an amp::span or array of such type.
template <typename RangeType, typename ElementType, typename = void>
struct IsNonSpanNonArrayRange : std::false_type
{
};
template <typename RangeType, typename ElementType>
struct IsNonSpanNonArrayRange<
    RangeType,
    ElementType,
    std::enable_if_t<
        amp::is_iterable<RangeType>::value && not(amp::is_span<RangeType>::value) &&
        not(std::is_array<RangeType>::value) &&
        std::is_same<std::remove_const_t<std::remove_reference_t<IteratorReferenceType<IteratorType<RangeType>>>>,
                     ElementType>::value>> : std::true_type
{
};
}  // namespace detail

/// \brief Helper type serving as view over string-like types.
/// \public
class LogString
{
  public:
    using TraitsType = amp::string_view::traits_type;
    using CharType = amp::string_view::value_type;
    using CharPtr = std::add_pointer_t<std::add_const_t<CharType>>;

    /// \brief Constructs `LogString` as view over character \p range;.
    ///
    /// This overload participates in overload resolution only if type `RangeType` is iterable via (const) `CharType`
    /// and is not an amp::span or an array of type `CharType`.
    ///
    template <typename RangeType,
              typename RT = amp::remove_cvref_t<RangeType>,
              std::enable_if_t<detail::IsNonSpanNonArrayRange<RT, CharType>::value, bool> = true>
    // NOLINTNEXTLINE(google-explicit-constructor): intended here to allow implicit conversions from range of `CharType`
    constexpr LogString(RangeType&& range) noexcept(noexcept(std::data(range)) && noexcept(std::size(range)))
        : data_{std::data(range)}, size_{std::size(range)}
    {
    }

    /// \brief Constructs `LogString` as view over character \p range;.
    ///
    /// This overload participates in overload resolution only if type `RangeType` is iterable via (const) `CharType`
    /// and is not an amp::span or an array of type `CharType`.
    ///
    template <typename RangeType,
              typename RT = amp::remove_cvref_t<RangeType>,
              std::enable_if_t<detail::IsNonSpanNonArrayRange<RT, CharType>::value, bool> = true>
    // NOLINTBEGIN(cppcoreguidelines-pro-type-member-init): false positive, this constructor delegates to another one
    // NOLINTNEXTLINE(google-explicit-constructor): intended here to allow implicit conversions from range of `CharType`
    constexpr LogString(std::reference_wrapper<RangeType> range_wrapper) noexcept(noexcept(LogString{
        range_wrapper.get()}))
        : LogString(range_wrapper.get())
    // NOLINTEND(cppcoreguidelines-pro-type-member-init): see justification above
    {
    }

    /// \brief Constructs `LogString` as view over a bounded character array.
    template <std::size_t N>
    // NOLINTNEXTLINE(google-explicit-constructor, modernize-avoid-c-arrays): allow implicit conversion from char array
    constexpr LogString(const CharType (&array)[N]) noexcept : data_{amp::data(array)}, size_{N - 1U}
    {
        static_assert(N > 0U, "character array must have at least 1 element");
        EnsureIsNullCharacter(array[N - 1U]);
    }

    /// \brief Constructs `LogString` as view over a character sequence pointed-to via `str`.
    constexpr LogString(CharPtr str, const std::size_t size) noexcept : data_{str}, size_{size} {}

    constexpr auto* Data() const noexcept { return data_; }
    constexpr auto Size() const noexcept { return size_; }

  private:
    /// \brief Performs assertion check whether a character value matches the null-character.
    constexpr static void EnsureIsNullCharacter(const CharType character) noexcept
    {
        AMP_ASSERT_PRD_MESSAGE(character == '\0', "character array must be null-terminated");
    }

    CharPtr data_;
    std::size_t size_;
};

/// \brief Helper type to log string and forward slog2 code.
/// \brief See QNX slog2f documentation.
/// \public
class LogSlog2Message
{
  public:
    LogSlog2Message() = delete;
    LogSlog2Message(const std::uint16_t code, const amp::string_view message) : slog_code_(code), message_(message) {}

    std::uint16_t GetCode() const { return slog_code_; }

    amp::string_view GetMessage() const { return message_; }

  private:
    std::uint16_t slog_code_;
    amp::string_view message_;
};

/// \brief Convenience method for logging character array
/// \public
/// \note Helps for example to avoid array-to-pointer decay when logging char[] literals or macros (such as __func__).
template <std::size_t N>
[[deprecated(
    "SPP_DEPRECATION: Making use of `mw::log::LogStr()` is no longer required since `mw::log::LogStream` "
    "meanwhile supports logging string literals natively via its `operator<<`.")]] constexpr LogString
// NOLINTNEXTLINE(modernize-avoid-c-arrays): required for avoiding implicit array-to-pointer decay in case of char array
LogStr(const LogString::CharType (&array)[N]) noexcept
{
    const LogString result{std::forward<decltype(array)>(array)};
    return result;
}

/// \brief Helper type to log the raw bytes of a buffer
/// \public
/// \note Maximum supported size for DLT output is less than 64 KB. Bytes exceeding that limit will be cropped.
/// \note Recommended to split the output in chunks of 1400 Bytes to avoid IP fragmentation DLT packets.
using LogRawBuffer =
    amp::span<const char>; 

/// \brief Create a LogRawBuffer from a scalar or array-of-scalars type instance
///
/// \tparam T Type of the data referenced by the value to be dumped.
/// \param values The value that shall be dumped.
/// \return An instance of LogRawBuffer, whose lifetime is limited by that of the given input reference.
template <typename T, typename = std::enable_if_t<std::is_scalar<T>::value>>
inline LogRawBuffer MakeLogRawBuffer(const T& value) noexcept
{
    // ----- COMMON_ARGUMENTATION ----
    // We cast to a char to print the byte representation of the given span. Since we know that:
    // - we do not violate the strict aliasing rules and
    // - we do not try to interpret or modify the data
    // the cast is acceptable.
    // --------------------------------

    
    // COMMON_ARGUMENTATION.
    // 
    return {reinterpret_cast<const char*>(  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast) COMMON_ARGUMENTATION
                &value),
            static_cast<LogRawBuffer::size_type>(sizeof(T))};
    
}

/// \brief Create a LogRawBuffer from an amp::span referencing an array of scalars.
///
/// \tparam T Type of the data referenced by the amp::span. Must be a scalar.
/// \param values The span referencing the array.
/// \return An instance of LogRawBuffer, whose lifetime is limited by that of the given input span.
template <typename T>
inline LogRawBuffer MakeLogRawBuffer(const amp::span<T> values) noexcept
{
    constexpr const bool IsEligible = std::is_scalar<T>::value;
    static_assert(IsEligible, "Only scalar types are allowed for dumping for now.");

    using SpanSizeType = typename amp::span<T>::size_type;
    
    AMP_PRECONDITION_MESSAGE(values.size() >= SpanSizeType{0}, "amp::span with negative size refused.");
    

    // ----- COMMON_ARGUMENTATION ----
    // We cast to a char to print the byte representation of the given span. Since we know that:
    // - the memory is contiguous and
    // - we do not violate the strict aliasing rules and
    // - we do not try to interpret or modify the data
    // the cast is acceptable.
    // -------------------------------

    
    
    /* other variants of the code would also require suppressions or were more complicated and not well-maintainable */
    const auto buffer_size{static_cast<LogRawBuffer::size_type>(static_cast<std::size_t>(values.size()) * sizeof(T))};
    // COMMON_ARGUMENTATION.
    // 
    return {reinterpret_cast<const char*>(  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast) COMMON_ARGUMENTATION
                values.data()),
            buffer_size};
    
    
}

/// \brief Create a LogRawBuffer from an std::vector referencing an array of scalars.
///
/// \tparam T Type of the data contained in the std::vector. Must be a scalar.
/// \param values The vector of elements.
/// \return An instance of LogRawBuffer, whose lifetime is limited by that of the given input vector.
template <typename T>
inline LogRawBuffer MakeLogRawBuffer(const std::vector<T>& values) noexcept
{
    return MakeLogRawBuffer(
        amp::span<const T>(values.data(), static_cast<typename amp::span<const T>::size_type>(values.size())));
}

/// \brief Create a LogRawBuffer from an std::array consisting of scalars.
///
/// \tparam T Type of the data contained in the std::array. Must be a scalar.
/// \param values The array of elements.
/// \return An instance of LogRawBuffer, whose lifetime is limited by that of the given input array.
template <typename T, std::size_t N>
inline LogRawBuffer MakeLogRawBuffer(const std::array<T, N>& values) noexcept
{
    return MakeLogRawBuffer(amp::span<const T>(values.data(), static_cast<typename amp::span<const T>::size_type>(N)));
}

}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif
