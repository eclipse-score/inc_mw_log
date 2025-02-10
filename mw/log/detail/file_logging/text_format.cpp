// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#include "platform/aas/mw/log/detail/file_logging/text_format.h"

#include "platform/aas/lib/memory/string_literal.h"

#include "amp_assert.hpp"
#include "amp_span.hpp"
#include "amp_utility.hpp"

#include <chrono>
#include <type_traits>

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

constexpr size_t kNumberOfBitsInByte = 8U;
constexpr size_t kTwoNibblesPerByte = 2U;
constexpr size_t kReserveSpaceForSpace = 1U;

//  global variable is unsynchronized and means at least so many hits:
std::size_t unsupported_types_count_hits = 0UL;


template <typename T>
std::size_t GetBufferSizeCasted(T buffer_size) noexcept
{
    //  We only intend to use conversion function with human readable messages
    //  plus final memory management method will be avoiding dynamic allocation
    //  which limits maximum buffer size
    static_assert(sizeof(T) <= sizeof(std::size_t), "Buffer size conversion error");
    return static_cast<std::size_t>(buffer_size);
}

std::size_t GetSpanSizeCasted(const amp::span<Byte> buffer) noexcept
{
    return GetBufferSizeCasted(buffer.size());
}

template <typename T, IntegerRepresentation I>
struct GetFormatSpecifier
{
};

//  to reuse the same mechanism and not to overcompilicate the code with additional template complexity,
//  IntegerRepresentation is still used for float and double types:
template <>
struct GetFormatSpecifier<const float, IntegerRepresentation::kDecimal>
{
    static constexpr bmw::StringLiteral value = "%f ";
};

template <>
struct GetFormatSpecifier<const double, IntegerRepresentation::kDecimal>
{
    static constexpr bmw::StringLiteral value = "%f ";
};

template <>
struct GetFormatSpecifier<const std::uint8_t, IntegerRepresentation::kHex>
{
    static constexpr bmw::StringLiteral value = "%hhx ";
};

template <>
struct GetFormatSpecifier<const std::uint8_t, IntegerRepresentation::kOctal>
{
    static constexpr bmw::StringLiteral value = "%hho ";
};

template <>
struct GetFormatSpecifier<const std::uint8_t, IntegerRepresentation::kDecimal>
{
    static constexpr bmw::StringLiteral value = "%hhu ";
};

template <>
struct GetFormatSpecifier<const std::uint16_t, IntegerRepresentation::kDecimal>
{
    static constexpr bmw::StringLiteral value = "%hu ";
};

template <>
struct GetFormatSpecifier<const std::uint16_t, IntegerRepresentation::kHex>
{
    static constexpr bmw::StringLiteral value = "%hx ";
};

template <>
struct GetFormatSpecifier<const std::uint16_t, IntegerRepresentation::kOctal>
{
    static constexpr bmw::StringLiteral value = "%ho ";
};

template <>
struct GetFormatSpecifier<const std::uint32_t, IntegerRepresentation::kDecimal>
{
    static constexpr bmw::StringLiteral value = "%u ";
};

template <>
struct GetFormatSpecifier<const std::uint32_t, IntegerRepresentation::kHex>
{
    static constexpr bmw::StringLiteral value = "%x ";
};

template <>
struct GetFormatSpecifier<const std::uint32_t, IntegerRepresentation::kOctal>
{
    static constexpr bmw::StringLiteral value = "%o ";
};

template <>
struct GetFormatSpecifier<const std::uint64_t, IntegerRepresentation::kDecimal>
{
    static constexpr bmw::StringLiteral value = "%lu ";
};

template <>
struct GetFormatSpecifier<const std::uint64_t, IntegerRepresentation::kHex>
{
    static constexpr bmw::StringLiteral value = "%lx ";
};

template <>
struct GetFormatSpecifier<const std::uint64_t, IntegerRepresentation::kOctal>
{
    static constexpr bmw::StringLiteral value = "%lo ";
};

template <>
struct GetFormatSpecifier<const std::int8_t, IntegerRepresentation::kDecimal>
{
    static constexpr bmw::StringLiteral value = "%hhi ";
};

template <>
struct GetFormatSpecifier<const std::int16_t, IntegerRepresentation::kDecimal>
{
    static constexpr bmw::StringLiteral value = "%hi ";
};

template <>
struct GetFormatSpecifier<const std::int32_t, IntegerRepresentation::kDecimal>
{
    static constexpr bmw::StringLiteral value = "%i ";
};

template <>
struct GetFormatSpecifier<const std::int64_t, IntegerRepresentation::kDecimal>
{
    static constexpr bmw::StringLiteral value = "%li ";
};

template <IntegerRepresentation I, typename T>
static void PutFormattedNumber(VerbosePayload& payload, const T data) noexcept
{
    amp::ignore = payload.Put([data](const amp::span<Byte> buffer) noexcept {
        const auto buffer_space = GetSpanSizeCasted(buffer);
        if (buffer_space > 0)  // LCOV_EXCL_BR_LINE: lcov complains about lots of uncovered branches here, it is not
                               // convenient/related to this condition.
        {
            constexpr bmw::StringLiteral format = GetFormatSpecifier<const T, I>::value;
            const auto written =
                // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg) safe to use std::snprintf
                FormattingFunctionReturnCast(std::snprintf(buffer.data(), GetSpanSizeCasted(buffer), format, data));

            const std::size_t last_index = std::min(written, GetSpanSizeCasted(buffer) - 1U);
            
            // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
            // False positive: Pointer arithmetic is used on span which is an array
            // 
            buffer.data()[last_index] = ' ';
            // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
            
            return written;
        }
        else
        {
            return 0UL;
        }
    });
}

template <typename T>
struct is_formatting_supported
{
    static constexpr bool value = std::is_unsigned<T>::value;
};

void HandleUnsupportedTypes() noexcept
{
    unsupported_types_count_hits++;
}


template <typename T, typename std::enable_if_t<is_formatting_supported<T>::value == false, bool> = true>
void PutBinaryFormattedNumber(VerbosePayload&, const T) noexcept
{
    //  Unsupported types action:
    HandleUnsupportedTypes();
}


template <typename T, typename std::enable_if_t<is_formatting_supported<T>::value, bool> = true>
void PutBinaryFormattedNumber(VerbosePayload& payload, const T data) noexcept
{
    constexpr auto characters_used = kNumberOfBitsInByte * sizeof(T) + kReserveSpaceForSpace;
    amp::ignore = payload.Put(
        [data](const amp::span<Byte> buffer) noexcept {
            const auto buffer_space = GetSpanSizeCasted(buffer);
            if (buffer_space > 1U)  // LCOV_EXCL_BR_LINE: lcov complains about lots of uncovered branches here, it is
                                    // not convenient/related to this condition.
            {
                constexpr auto number_of_bits = kNumberOfBitsInByte * sizeof(T);
                const auto max_possible = std::min(number_of_bits, buffer_space);
                std::size_t buffer_index = 0;
                
                // LCOV_EXCL_BR_START: The loop condition is always true because buffer_index starts at 0 and
                // max_possible is always greater than 1 under this condition if (buffer_space > 1U). And there is no
                // way to make it false. Therefore, we can safely suppress the coverage warning for this decision.
                while (buffer_index < max_possible)  //  only MSB bits will be filled in case of insufficient memory
                // LCOV_EXCL_BR_STOP
                {
                    const auto bits = std::bitset<number_of_bits>(data);
                    // bit [] access can only have 0 or 1 return value and even if offset by value of '0'
                    // is not able to overflow char
                    // static_cast<std::int32_t> is needed to perform addition '0' char value to bool value from
                    // std::bitset static_cast<std::string::value_type> because this is the type of the content of our
                    // buffer
                    
                    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
                    // False positive: Pointer arithmetic is used on span which is an array
                    // 
                    buffer.data()[buffer_index] = static_cast<std::string::value_type>(
                        static_cast<std::int32_t>('0') + bits[bits.size() - 1U - buffer_index]);
                    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
                    
                    buffer_index++;
                }
                

                const std::size_t last_index = buffer_space - 1U;
                
                // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
                // False positive: Pointer arithmetic is used on span which is an array
                // 
                buffer.data()[last_index] = ' ';
                // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
                
                return buffer_index + kReserveSpaceForSpace;
            }
            else
            {
                return 0UL;
            }
        },
        characters_used);
}
}  // namespace

std::size_t FormattingFunctionReturnCast(const std::int32_t i) noexcept
{
    if (i > 0)
    {
        return GetBufferSizeCasted(i);
    }
    return std::size_t{0U};
}

void TextFormat::PutFormattedTime(VerbosePayload& payload) noexcept
{
    const auto time_point = std::chrono::system_clock::now();
    amp::ignore = payload.Put([time_point](const amp::span<Byte> buffer) noexcept {
        std::size_t total = 0;
        const auto now = std::chrono::system_clock::to_time_t(time_point);
        struct tm time_structure_buffer
        {
        };
        const struct tm* const time_structure =
            localtime_r(&now, &time_structure_buffer);  // LCOV_EXCL_BR_LINE: there are no branches to be covered.
        if (nullptr != time_structure)  // LCOV_EXCL_BR_LINE: "nullptr" condition can't be controlled via test case.
        {
            const std::size_t buffer_space = GetSpanSizeCasted(buffer);
            
            /* False positive: Pointer arithmetic is used on span which is an array. */
            /* Stdlib time library is used just for formatting headers of logs. It is not critical feature */
            // NOLINTNEXTLINE(bmw-banned-function) justified above
            const auto written = std::strftime(buffer.data(), buffer_space, "%Y/%m/%d %H:%M:%S", time_structure);
            

            total += written;
            if (buffer_space > total)
            {
                const std::size_t last_index = std::min(written, buffer_space - 1U);
                // Cast to ptrdiff_t is intended according to amp::at signature. The variable "buffer" is taken from
                // amp::span, which size is limited to the positive values of std::ptrdiff_t.
                amp::at(buffer, static_cast<std::ptrdiff_t>(last_index)) =
                    '.';  // LCOV_EXCL_BR_LINE: there are no branches to be covered.
                total += 1U;
            }
        }
        return total;
    });

    const auto time_elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(time_point.time_since_epoch()).count() % 10'000'000;

    // time_elapsed values are within the scope from 0 to 9'999'999. Casting to uint32_t is safe then.
    const auto time_structure_elapsed = static_cast<std::uint32_t>(time_elapsed);

    PutFormattedNumber<IntegerRepresentation::kDecimal>(payload, time_structure_elapsed);
}


template <typename T, typename std::enable_if_t<is_formatting_supported<T>::value == false, bool> = true>
void PutHexFormattedNumber(VerbosePayload&, const T) noexcept
{
    //  Unsupported types action:
    HandleUnsupportedTypes();
}


template <typename T, typename std::enable_if_t<is_formatting_supported<T>::value, bool> = true>
void PutHexFormattedNumber(VerbosePayload& payload, const T data) noexcept
{
    PutFormattedNumber<IntegerRepresentation::kHex>(payload, data);
}


template <typename T, typename std::enable_if_t<is_formatting_supported<T>::value == false, bool> = true>
void PutOctalFormattedNumber(VerbosePayload&, const T) noexcept
{
    //  Unsupported types action:
    HandleUnsupportedTypes();
}


template <typename T, typename std::enable_if_t<is_formatting_supported<T>::value, bool> = true>
void PutOctalFormattedNumber(VerbosePayload& payload, const T data) noexcept
{
    PutFormattedNumber<IntegerRepresentation::kOctal>(payload, data);
}

template <typename T>
static void LogData(VerbosePayload& payload,
                    const T data,
                    const IntegerRepresentation integral_representation = IntegerRepresentation::kDecimal) noexcept
{
    switch (integral_representation)
    {
        case IntegerRepresentation::kHex:
            PutHexFormattedNumber(payload, data);
            break;
        case IntegerRepresentation::kBinary:
            PutBinaryFormattedNumber(payload, data);
            break;
        case IntegerRepresentation::kOctal:
            PutOctalFormattedNumber(payload, data);
            break;
        case IntegerRepresentation::kDecimal:
        default:
            PutFormattedNumber<IntegerRepresentation::kDecimal>(payload, data);
            break;
    }
}

void TextFormat::Log(VerbosePayload& payload, const bool data) noexcept
{
    constexpr auto positive_value = amp::string_view{"True"};
    constexpr auto negative_value = amp::string_view{"False"};

    bmw::mw::log::detail::TextFormat::Log(payload, data ? positive_value : negative_value);
}

void TextFormat::Log(VerbosePayload& payload,
                     const std::uint8_t data,
                     const IntegerRepresentation integral_representation) noexcept
{
    bmw::mw::log::detail::LogData(payload, data, integral_representation);
}

void bmw::mw::log::detail::TextFormat::Log(VerbosePayload& payload,
                                           const std::uint16_t data,
                                           const IntegerRepresentation integral_representation) noexcept
{
    bmw::mw::log::detail::LogData(payload, data, integral_representation);
}

void bmw::mw::log::detail::TextFormat::Log(VerbosePayload& payload,
                                           const std::uint32_t data,
                                           const IntegerRepresentation integral_representation) noexcept
{
    bmw::mw::log::detail::LogData(payload, data, integral_representation);
}

void bmw::mw::log::detail::TextFormat::Log(VerbosePayload& payload,
                                           const std::uint64_t data,
                                           const IntegerRepresentation integral_representation) noexcept
{
    bmw::mw::log::detail::LogData(payload, data, integral_representation);
}

void bmw::mw::log::detail::TextFormat::Log(VerbosePayload& payload,
                                           const std::int8_t data,
                                           const IntegerRepresentation integral_representation) noexcept
{
    bmw::mw::log::detail::LogData(payload, data, integral_representation);
}

void bmw::mw::log::detail::TextFormat::Log(VerbosePayload& payload,
                                           const std::int16_t data,
                                           const IntegerRepresentation integral_representation) noexcept
{
    bmw::mw::log::detail::LogData(payload, data, integral_representation);
}

void bmw::mw::log::detail::TextFormat::Log(VerbosePayload& payload,
                                           const std::int32_t data,
                                           const IntegerRepresentation integral_representation) noexcept
{
    bmw::mw::log::detail::LogData(payload, data, integral_representation);
}

void bmw::mw::log::detail::TextFormat::Log(VerbosePayload& payload,
                                           const std::int64_t data,
                                           const IntegerRepresentation integral_representation) noexcept
{
    bmw::mw::log::detail::LogData(payload, data, integral_representation);
}

void bmw::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const LogHex8 data) noexcept
{
    bmw::mw::log::detail::LogData(payload, data.value, IntegerRepresentation::kHex);
}

void bmw::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const LogHex16 data) noexcept
{
    bmw::mw::log::detail::LogData(payload, data.value, IntegerRepresentation::kHex);
}

void bmw::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const LogHex32 data) noexcept
{
    bmw::mw::log::detail::LogData(payload, data.value, IntegerRepresentation::kHex);
}

void bmw::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const LogHex64 data) noexcept
{
    bmw::mw::log::detail::LogData(payload, data.value, IntegerRepresentation::kHex);
}

void bmw::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const LogBin8 data) noexcept
{
    bmw::mw::log::detail::LogData(payload, data.value, IntegerRepresentation::kBinary);
}

void bmw::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const LogBin16 data) noexcept
{
    bmw::mw::log::detail::LogData(payload, data.value, IntegerRepresentation::kBinary);
}

void bmw::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const LogBin32 data) noexcept
{
    bmw::mw::log::detail::LogData(payload, data.value, IntegerRepresentation::kBinary);
}

void bmw::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const LogBin64 data) noexcept
{
    bmw::mw::log::detail::LogData(payload, data.value, IntegerRepresentation::kBinary);
}

void bmw::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const float data) noexcept
{
    bmw::mw::log::detail::LogData(payload, data);
}

void bmw::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const double data) noexcept
{
    bmw::mw::log::detail::LogData(payload, data);
}

void bmw::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const amp::string_view data) noexcept
{
    if (data.size() > 0)
    {
        const std::size_t data_length = data.size() + kReserveSpaceForSpace;

        amp::ignore = payload.Put(
            [data](const amp::span<Byte> buffer) {
                const std::size_t length = std::min(data.size(), GetSpanSizeCasted(buffer));
                if (length > 0)  // LCOV_EXCL_BR_LINE: Cannot be covered in unit tests as 'data.size() > 0' and buffer
                                 // size is uncontrollable.
                {
                    // NOLINTNEXTLINE(bmw-banned-function) memcpy is needed to copy string_view data
                    amp::ignore = std::memcpy(buffer.data(), data.data(), length);
                    const std::size_t last_index = GetSpanSizeCasted(buffer) - 1U;
                    
                    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
                    // False positive: Pointer arithmetic is used on span which is an array
                    // 
                    buffer.data()[last_index] = ' ';
                    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
                    
                    return GetSpanSizeCasted(buffer);
                }
                return 0LU;  // LCOV_EXCL_LINE: Cannot be covered in unit tests as if (length > 0) is uncontrollable.
            },
            data_length);
    }
}

void bmw::mw::log::detail::TextFormat::TerminateLog(VerbosePayload& payload) noexcept
{
    payload.Put("\n", kReserveSpaceForSpace);
}

void bmw::mw::log::detail::TextFormat::Log(VerbosePayload& payload, const LogRawBuffer data) noexcept
{
    const auto max_string_len = kTwoNibblesPerByte * GetBufferSizeCasted(data.size());

    if (max_string_len > std::size_t{0U})
    {
        amp::ignore = payload.Put(
            [data](const amp::span<Byte> buffer) noexcept {
                std::size_t total{0};
                std::size_t i{0};
                std::size_t space_left{GetBufferSizeCasted(buffer.size())};
                auto data_source = data.data();
                //  proceed if some space is in the buffer and there are still some elements to convert
                // LCOV_EXCL_BR_START: The (i < GetBufferSizeCasted(data.size())) condition is always true under current
                // logic because 'i' starts at 0 and 'data.size()' is greater than 0 when 'max_string_len > 0' is true.
                // Therefore, we can suppress the coverage warning for this decision.
                while ((space_left > std::size_t{0U}) && (i < GetBufferSizeCasted(data.size())))
                // LCOV_EXCL_BR_STOP
                {
                    auto data_iter = buffer.data();
                    std::advance(data_iter, static_cast<std::ptrdiff_t>(total));
                    const auto written = FormattingFunctionReturnCast(
                        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg) safe to use std::snprintf
                        std::snprintf(data_iter, space_left, "%02hhx", *data_source));
                    total += written;
                    space_left -= written;
                    std::advance(data_source, 1UL);
                    i++;
                }
                //  if space left put space character after data
                //  else overwrite last character with space
                if (space_left > std::size_t{0U})
                {
                    total += std::size_t{1U};
                }
                if (total > std::size_t{0U})
                {
                    
                    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
                    // False positive: Pointer arithmetic is used on span which is an array, bound checking done
                    // 
                    buffer.data()[total - std::size_t{1U}] = ' ';
                    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
                    
                }

                return total;
            },
            max_string_len + kReserveSpaceForSpace);
    }
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
