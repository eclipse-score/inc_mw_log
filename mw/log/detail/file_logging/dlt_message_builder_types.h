// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#ifndef PLATFORM_AAS_MW_LOG_DETAIL_DLT_MESSAGE_BUILDER_TYPES_H_
#define PLATFORM_AAS_MW_LOG_DETAIL_DLT_MESSAGE_BUILDER_TYPES_H_

#include <array>
#include <cstdint>
#include <limits>

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{




#ifndef PACKED
#define PACKED __attribute__((aligned(1), packed))
#endif





constexpr std::uint32_t kDltMsinVerb{0x01};   /**< verbose */
constexpr std::uint32_t kDltMsinMstpShift{1}; /**< shift right offset to get mstp value */
constexpr std::uint32_t kDltMsinMtinShift{4}; /**< shift right offset to get mtin value */

/*
 * Definitions of the htyp parameter in standard header.
 */
constexpr std::uint32_t kDltHtypUEH{0x01UL};  /**< use extended header */
constexpr std::uint32_t kDltHtypWEID{0x04UL}; /**< with ECU ID */
constexpr std::uint32_t kDltHtypWTMS{0x10UL}; /**< with timestamp */
constexpr std::uint32_t kDltHtypVERS{0x20UL}; /**< version number, 0x1 */

static_assert((kDltMsinVerb | kDltHtypUEH | kDltHtypWEID | kDltHtypWTMS | kDltHtypVERS) <=
                  std::numeric_limits<std::uint8_t>::max(),
              "Flag values out of range");

// Cast from int to uint32_t to avoid warning about comparison between signed and unsigned and to prevent implicit
// conversion. This cast is safe for below cases because the variable 'digits' is always positive, and the type 'int' is
// mandated by C++ standard.
static_assert(kDltMsinMstpShift < static_cast<std::uint32_t>(std::numeric_limits<std::uint8_t>::digits),
              "Shift operator out of range");
static_assert(kDltMsinMtinShift < static_cast<std::uint32_t>(std::numeric_limits<std::uint8_t>::digits),
              "Shift operator out of range");

/*
 * Definitions of mstp parameter in extended header.
 */
constexpr std::uint32_t kDltTypeLOG{0x00}; /**< Log message type */

constexpr std::size_t kDltIdSize = 4UL;
constexpr std::size_t kDltMessageSize = 65535UL;
constexpr std::size_t kDltHeaderSize = 22UL;
constexpr std::size_t kDltStorageHeaderSize = 16UL;

constexpr std::size_t kMaxDltHeaderSize = 512UL;


// NOLINTBEGIN(bmw-banned-preprocessor-directives) needed to use PACKED attribute
#if defined(__GNUC__)
// needed to use PACKED attribute
// 
#pragma GCC diagnostic push
// needed to use PACKED attribute
// 
#pragma GCC diagnostic ignored "-Wpacked"
// needed to use PACKED attribute
// 
#pragma GCC diagnostic ignored "-Wattributes"
#endif
// NOLINTEND(bmw-banned-preprocessor-directives) needed to use PACKED attribute


/**
 * The structure of the DLT standard header. This header is used in each DLT message.
 * Names of the members are following AUTOSAR specification field names.
 */
struct DltStandardHeader
{
    uint8_t htyp; /**< This parameter contains several informations, see definitions below */
    uint8_t mcnt; /**< The message counter is increased with each sent DLT message */
    uint16_t len; /**< Length of the complete message, without storage header */
} PACKED;
/**
 * The structure of the DLT file storage header. This header is used before each stored DLT message.
 */
struct DltStorageHeader
{
    std::array<std::uint8_t, kDltIdSize> pattern; /**< This pattern should be DLT0x01 */
    uint32_t seconds;                             /**< seconds since 1.1.1970 */
    int32_t microseconds;                         /**< Microseconds */
    std::array<std::uint8_t, kDltIdSize>
        ecu; /**< The ECU id is added, if it is not already in the DLT message itself */
} PACKED;

/**
 * The structure of the DLT extra header parameters. Each parameter is sent only if enabled in htyp.
 * Names of the members are following AUTOSAR specification field names.
 */
struct DltStandardHeaderExtra
{
    std::array<std::uint8_t, kDltIdSize> ecu; /**< ECU id */
    uint32_t tmsp;                            /**< Timestamp since system start in 0.1 milliseconds */
} PACKED;

/**
 * The structure of the DLT extended header. This header is only sent if enabled in htyp parameter.
 * Names of the members are following AUTOSAR specification field names.
 */
struct DltExtendedHeader
{
    uint8_t msin;                              /**< messsage info */
    uint8_t noar;                              /**< number of arguments */
    std::array<std::uint8_t, kDltIdSize> apid; /**< application id */
    std::array<std::uint8_t, kDltIdSize> ctid; /**< context id */
} PACKED;

struct DltVerboseHeader
{
    DltStandardHeader standard;
    DltStandardHeaderExtra extra;
    DltExtendedHeader extended;
} PACKED;


#if defined(__GNUC__)
// NOLINTBEGIN(bmw-banned-preprocessor-directives) needed to use PACKED attribute
// needed to use PACKED attribute
// 
#pragma GCC diagnostic pop
// NOLINTEND(bmw-banned-preprocessor-directives) needed to use PACKED attribute
#endif

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  //  PLATFORM_AAS_MW_LOG_DETAIL_DLT_MESSAGE_BUILDER_TYPES_H_
