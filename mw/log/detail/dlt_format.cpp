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


#include "platform/aas/mw/log/detail/dlt_format.h"

#include "platform/aas/lib/bitmanipulation/bit_manipulation.h"

#include "amp_assert.hpp"
#include "amp_span.hpp"
#include <amp_optional.hpp>
#include <amp_utility.hpp>

#include <type_traits>

/// For specification of the DLT protocol, please see:
/// https://www.autosar.org/fileadmin/user_upload/standards/foundation/1-0/AUTOSAR_PRS_DiagnosticLogAndTraceProtocol.pdf
///
/// As described in chapter 5.1 the general format of an DLT message looks as follows:
/// +-----------------+-----------------+---------+
/// | Standard Header | Extended Header | Payload |
/// +-----------------+-----------------+---------+
///
/// The `Standard Header` and `Extended Header` are for now no concern in this implementation. They will be filled by
/// the `DataRouter` application. For now we focus in this document on the `Payload` part.
///
/// The payload section can be filled in two ways: Non-Verbose (chapter 5.1.2.1) or Verbose (5.1.2.2).
/// This part of our code only implements the Verbose-Mode.
///
/// The verbose mode is further split into argument sections (PRS_Dlt_00459).
/// +-----------------+-----------------+-----------------------------------------------------+
/// | Standard Header | Extended Header |                       Payload                       |
/// |                 |                 +--------------------------+--------------------------+
/// |                 |                 |        Argument 1        |        Argument 2        |
/// |                 |                 +-----------+--------------+-----------+--------------+
/// |                 |                 | Type Info | Data Payload | Type Info | Data Payload |
/// +-----------------+-----------------+-----------+--------------+-----------+--------------+
///
/// For now also this handling of argument numbers is not handled within this class. This class rather interprets each
/// call to Log() as another argument that is added towards the payload. For this it takes case that the Type Info and
/// Data Payload are filled correctly. Any recorder using this formatter, will for now take care of the argument
/// handling.

namespace
{

// \Requirement PRS_Dlt_00626, PRS_Dlt_00354
enum class TypeLength : std::uint32_t
{
    kNotDefined = 0x00,
    k8Bit = 0x01,
    k16Bit = 0x02,
    k32Bit = 0x03,
    k64Bit = 0x04,
    k128Bi = 0x05,
};

// \Requirement PRS_Dlt_00627, PRS_Dlt_00182, PRS_Dlt_00366
enum class StringEncoding : std::uint32_t
{
    kASCII = 0x00,
    kUTF8 = 0x01,
};

// \Requirement PRS_Dlt_00783
enum class IntegerRepresentation : std::uint32_t
{
    kBase10 = 0x00,
    kBase8 = 0x01,
    kBase16 = 0x02,
    kBase2 = 0x03
};

class TypeInfo
{
  public:
    explicit TypeInfo(const std::uint32_t type)
    {
        amp::ignore = bmw::platform::SetBit(underlying_type_, static_cast<std::size_t>(type));
    }

    // \Requirement PRS_Dlt_00625
    constexpr static std::size_t TYPE_BOOL_BIT = 4U;
    constexpr static std::size_t TYPE_SIGNED_BIT = 5U;
    constexpr static std::size_t TYPE_UNSIGNED_BIT = 6U;
    constexpr static std::size_t TYPE_FLOAT_BIT = 7U;
    constexpr static std::size_t TYPE_STRING_BIT = 9U;
    constexpr static std::size_t TYPE_RAW_BIT = 10U;

    // \Requirement PRS_Dlt_00354
    amp::optional<TypeInfo> Set(const TypeLength length)
    {
        static_assert(std::is_same<std::underlying_type<TypeLength>::type, std::uint32_t>::value,
                      "Mismatching underlying type. Cast not valid.");
        underlying_type_ |= static_cast<std::uint32_t>(length);
        return *this;
    }

    // \Requirement PRS_Dlt_00183, PRS_Dlt_00367
    amp::optional<TypeInfo> Set(const StringEncoding encoding) noexcept
    {
        const auto is_type_string_bit_set = bmw::platform::CheckBit(underlying_type_, TYPE_STRING_BIT);
        const auto is_trace_info_bit_set = bmw::platform::CheckBit(underlying_type_, TRACE_INFO_BIT);
        
        
        if ((is_type_string_bit_set || is_trace_info_bit_set) == false)
        {
            return {};
        }
        
        

        static_assert(std::is_same<std::underlying_type<StringEncoding>::type, std::uint32_t>::value,
                      "Mismatching underlying type. Cast not valid.");
        underlying_type_ |= (static_cast<std::uint32_t>(encoding) << STRING_ENCODING_START);
        return *this;
    }

    // \Requirement PRS_Dlt_00782, PRS_Dlt_00783
    amp::optional<TypeInfo> Set(const bmw::mw::log::detail::IntegerRepresentation encoding) noexcept
    {
        const auto is_type_unsigned_bit_set = bmw::platform::CheckBit(underlying_type_, TYPE_UNSIGNED_BIT);
        const auto is_type_signed_bit_set = bmw::platform::CheckBit(underlying_type_, TYPE_SIGNED_BIT);
        
        
        if ((is_type_unsigned_bit_set || is_type_signed_bit_set) ==
            false)  // LCOV_EXCL_BR_LINE: can't achieve the other conditions because can't control the
                    // "underlying_type_" value.
        {
            return {};
        }
        
        

        // Make sure the enum values match the values from the standard requirement PRS_Dlt_00783.
        static_assert(static_cast<std::uint8_t>(IntegerRepresentation::kBase10) == 0U,
                      "Value shall match PRS_Dlt_00783");
        static_assert(static_cast<std::uint8_t>(IntegerRepresentation::kBase8) == 1U,
                      "Value shall match PRS_Dlt_00783");
        static_assert(static_cast<std::uint8_t>(IntegerRepresentation::kBase16) == 2U,
                      "Value shall match PRS_Dlt_00783");
        static_assert(static_cast<std::uint8_t>(IntegerRepresentation::kBase2) == 3U,
                      "Value shall match PRS_Dlt_00783");

        static_assert(
            std::is_same<std::underlying_type<bmw::mw::log::detail::IntegerRepresentation>::type, std::uint8_t>::value,
            "Mismatching underlying type. Cast not valid.");
        underlying_type_ |= (static_cast<std::uint32_t>(encoding) << INTEGER_ENCODING_START);
        return *this;
    }

  private:
    // \Requirement PRS_Dlt_00135
    std::uint32_t underlying_type_{};

    
    // \Requirement PRS_Dlt_00625
    // constexpr static auto VARIABLE_INFO_BIT = 11U; not supported in our implementation
    // constexpr static auto FIXED_POINT_BIT = 12U; not supported in our implementation
    constexpr static std::size_t TRACE_INFO_BIT{13};
    constexpr static auto STRING_ENCODING_START = 15U;
    // \Requirement PRS_Dlt_00782
    constexpr static auto INTEGER_ENCODING_START = 15U;
    
};

template <typename T>
std::size_t Sum(T t)
{
    return t;
}

template <typename T, typename... Rest>
std::size_t Sum(T t, Rest... rest)
{
    return t + Sum(rest...);
}


template <typename T>
std::size_t SizeOf(T)
{
    return sizeof(T);
}


std::size_t SizeOf(const amp::string_view t)
{
    return t.size();
}

std::size_t SizeOf(const bmw::mw::log::LogRawBuffer t)
{
    return static_cast<std::size_t>(t.size());
}

template <typename... T>
bool WillMessageFit(bmw::mw::log::detail::VerbosePayload& payload, T... message_parts)
{
    std::size_t size{};
    size = Sum(SizeOf(message_parts)...);

    return !payload.WillOverflow(size);
}

using ByteView = amp::span<const bmw::mw::log::detail::Byte>;

template <typename T>
ByteView ToByteView(const T& t)
{
    // Yes, this is bad, but its what we want. We want to store the current plain memory
    // byte by byte in our buffer. Thus, this reinterpret cast is the only correct way to-do this.
    
    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast) justified above
    // reinterpret_cast needed to store current plain memory in logs
    // 
    return ByteView{reinterpret_cast<const bmw::mw::log::detail::Byte*>(&t), sizeof(t)};
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast) justified above
    
}

ByteView ToByteView(const amp::string_view& t)
{
    return ByteView{t.data(), static_cast<ByteView::size_type>(t.size())};
}

ByteView ToByteView(const bmw::mw::log::LogRawBuffer& t)
{
    return ByteView{t.data(), t.size()};
}

template <class F, class First, class... Rest>
void DoFor(F function, First first, Rest... rest)
{
    function(ToByteView(first));

    DoFor(function, rest...);
}


//  coverage: coverage shown by manual inspection.
//  Reasoning: empty function doesn't produce any code to be covered.
//  See also: 
template <class F>
void DoFor(F)
{
    // Parameter pack is empty.
}


template <typename... T>
bmw::mw::log::detail::AddArgumentResult Store(bmw::mw::log::detail::VerbosePayload& payload, T... data_for_payload)
{
    if (WillMessageFit(payload, data_for_payload...) == true)
    {
        
        /* The lambda will be executed within this stack. Thus, all references are still valid */
        DoFor(
            [&payload](const ByteView byte_view) {
                payload.Put(byte_view.data(), static_cast<std::size_t>(byte_view.size()));
            },
            data_for_payload...);
        
        return bmw::mw::log::detail::AddArgumentResult::Added;
    }
    return bmw::mw::log::detail::AddArgumentResult::NotAdded;
}

bmw::mw::log::detail::AddArgumentResult TryStore(bmw::mw::log::detail::VerbosePayload& payload,
                                                 const TypeInfo& type_info,
                                                 const std::uint64_t max_string_len_incl_null,
                                                 const amp::string_view data) noexcept
{
    const std::uint64_t max_string_len = max_string_len_incl_null - std::size_t{1U};
    const auto length_cropped = static_cast<std::uint16_t>(std::min(data.size(), max_string_len));
    const auto length_incl_null = static_cast<std::uint16_t>(length_cropped + 1U);

    const amp::string_view data_cropped{data.data(), length_cropped};
    return Store(payload, type_info, length_incl_null, data_cropped, '\0');
}

template <typename Resolution>
bmw::mw::log::detail::AddArgumentResult LogData(bmw::mw::log::detail::VerbosePayload& payload,
                                                const Resolution data,
                                                const bmw::mw::log::detail::IntegerRepresentation repr,
                                                const std::uint32_t type,
                                                TypeLength type_length) noexcept
{
    // \Requirement PRS_Dlt_00386, PRS_Dlt_00356, PRS_Dlt_00358
    TypeInfo type_info(type);
    if ((type_info.Set(type_length).has_value() == false) || (type_info.Set(repr).has_value() == false))
    {
        return bmw::mw::log::detail::AddArgumentResult::NotAdded;
    }

    // \Requirement PRS_Dlt_00370
    return Store(payload, type_info, data);
}
}  // namespace

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{


AddArgumentResult DLTFormat::Log(VerbosePayload& payload, const bool data) noexcept
{
    // \Requirement PRS_Dlt_00139
    TypeInfo type_info(TypeInfo::TYPE_BOOL_BIT);
    if (type_info.Set(TypeLength::k8Bit).has_value() == false)
    {
        return AddArgumentResult::NotAdded;
    }

    // \Requirement PRS_Dlt_00422
    
    static_assert((sizeof(data) == std::size_t{1U}) == true, "Bool is not of size one Byte");
    

    // \Requirement PRS_Dlt_00369, PRS_Dlt_00423
    return Store(payload, type_info, data);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const std::uint8_t data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k8Bit);
}



AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const std::uint16_t data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k16Bit);
}



AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const std::uint32_t data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k32Bit);
}



AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const std::uint64_t data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k64Bit);
}



AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const std::int8_t data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_SIGNED_BIT, TypeLength::k8Bit);
}



AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const std::int16_t data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_SIGNED_BIT, TypeLength::k16Bit);
}



AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const std::int32_t data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_SIGNED_BIT, TypeLength::k32Bit);
}



AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const std::int64_t data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_SIGNED_BIT, TypeLength::k64Bit);
}



AddArgumentResult DLTFormat::Log(VerbosePayload& payload, const LogHex8 data, const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k8Bit);
}



AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const LogHex16 data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k16Bit);
}



AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const LogHex32 data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k32Bit);
}



AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const LogHex64 data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k64Bit);
}



AddArgumentResult DLTFormat::Log(VerbosePayload& payload, const LogBin8 data, const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k8Bit);
}



AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const LogBin16 data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k16Bit);
}



AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const LogBin32 data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k32Bit);
}



AddArgumentResult DLTFormat::Log(VerbosePayload& payload,
                                 const LogBin64 data,
                                 const IntegerRepresentation repr) noexcept
{
    return LogData(payload, data, repr, TypeInfo::TYPE_UNSIGNED_BIT, TypeLength::k64Bit);
}


AddArgumentResult DLTFormat::Log(VerbosePayload& payload, const float data) noexcept
{
    // \Requirement PRS_Dlt_00390, PRS_Dlt_00145
    TypeInfo type_info(TypeInfo::TYPE_FLOAT_BIT);
    if (type_info.Set(TypeLength::k32Bit).has_value() == false)
    {
        return AddArgumentResult::NotAdded;
    }

    // \Requirement PRS_Dlt_00371
    return Store(payload, type_info, data);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload, const double data) noexcept
{
    // \Requirement PRS_Dlt_00386, PRS_Dlt_00356
    TypeInfo type_info(TypeInfo::TYPE_FLOAT_BIT);
    if (type_info.Set(TypeLength::k64Bit).has_value() == false)
    {
        return AddArgumentResult::NotAdded;
    }

    // \Requirement PRS_Dlt_00371
    return Store(payload, type_info, data);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload, const amp::string_view data) noexcept
{
    // \Requirement PRS_Dlt_00420, PRS_Dlt_00155
    TypeInfo type_info(TypeInfo::TYPE_STRING_BIT);
    if (type_info.Set(StringEncoding::kUTF8).has_value() == false)
    {
        return AddArgumentResult::NotAdded;
    }

    // \Requirement PRS_Dlt_00156, PRS_Dlt_00373
    // The string payload shall be assembled as follows:
    //       _____________________________________________
    //      |16-bit Length including termination character|
    //      |_____________________________________________|
    //      |Encoded data string length < 2^16 bytes      |
    //      |_____________________________________________|
    //      | Zero terminator 1 byte                      |
    //      |_____________________________________________|
    // Note that in pratice the string must be even shorter as the entire DLT message must fit in max 2^16 bytes
    // including the DLT headers.

    // Number of bytes needed for the header needed before the payload.
    constexpr std::uint64_t header_size = sizeof(type_info) + sizeof(std::uint16_t);

    if (payload.RemainingCapacity() <= header_size)
    {
        // No space left in buffer for payload.
        return AddArgumentResult::NotAdded;
    }

    // Calculate the length including the terminator.
    constexpr auto uint16_max = static_cast<std::uint64_t>(std::numeric_limits<std::uint16_t>::max());
    const std::uint64_t max_string_len_incl_null = std::min(payload.RemainingCapacity() - header_size, uint16_max);

    if (max_string_len_incl_null == 0)  // LCOV_EXCL_BR_LINE: the condition should not be "true" according to the below
                                        // comments. Excluded from branch coverage.
    {
        
        /* Tolerated by decision. The part of the code, which should never be reached. Excluded from code coverage as
         * well. */
        
        /* Tolerated by decision. The part of the code, which should never be reached. Excluded from code coverage as
         * well. */
        std::abort();  // LCOV_EXCL_LINE defensive programming: Only defined ParsingPhase values are possible to be
                       // reached.
        
        /* Tolerated by decision. The part of the code, which should never be reached. Excluded from code coverage as
         * well. */
        
        /* Tolerated by decision. The part of the code, which should never be reached. Excluded from code coverage as
         * well. */
    }

    return TryStore(payload, type_info, max_string_len_incl_null, data);
}

AddArgumentResult DLTFormat::Log(VerbosePayload& payload, const LogRawBuffer data) noexcept
{
    // \Requirement PRS_Dlt_00625
    const auto type_info = TypeInfo(TypeInfo::TYPE_RAW_BIT);

    // \Requirement PRS_Dlt_00160, PRS_Dlt_00374
    // The string payload shall be assembled as follows:
    //       _____________________________________________
    //      |16-bit Length                                |
    //      |_____________________________________________|
    //      |Data string length <= 2^16 bytes             |
    //      |_____________________________________________|
    // Note that in pratice the data must be even shorter as the entire DLT message must fit in max 2^16 bytes
    // including the DLT headers.

    // Number of bytes needed for the header needed before the payload.
    constexpr std::uint64_t header_size = sizeof(type_info) + sizeof(std::uint16_t);

    if (payload.RemainingCapacity() <= header_size)
    {
        // No space left in buffer for payload.
        return AddArgumentResult::NotAdded;
    }

    // Calculate the possibly cropped length
    constexpr auto uint16_max = static_cast<std::uint64_t>(std::numeric_limits<std::uint16_t>::max());
    const std::uint64_t max_length = std::min(payload.RemainingCapacity() - header_size, uint16_max);
    const std::uint16_t length_cropped =
        static_cast<std::uint16_t>(std::min(static_cast<std::uint64_t>(data.size()), max_length));

    const LogRawBuffer data_cropped{data.data(), length_cropped};
    return Store(payload, type_info, length_cropped, data_cropped);
}


}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
