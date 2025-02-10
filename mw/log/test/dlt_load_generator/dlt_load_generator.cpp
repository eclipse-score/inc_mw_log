// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************




#include "platform/aas/mw/log/logger.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#if VERBOSE_LOGGING
#else
#include "platform/aas/mw/log/legacy_non_verbose_api/tracing.h"
#endif

using namespace std::chrono_literals;

namespace
{
constexpr auto kChunkDuration = 100ms;
constexpr auto kDltMessagePayloadSizeBytes = 1400;
constexpr auto kStimulationDurationSeconds = 60s;
}  // namespace

#if VERBOSE_LOGGING
#else

namespace bmw
{
namespace logging
{
namespace internal
{
struct NonVerboseMessage
{
    std::uint8_t array[kDltMessagePayloadSizeBytes];
};
STRUCT_TRACEABLE(bmw::logging::internal::NonVerboseMessage, array)

}  // namespace internal
}  // namespace logging
}  // namespace bmw

#endif

int main(int argc, char** argv)
{

    if (argc != 2)
    {
        std::cout << "Usage: bin/load_test <load in MB/s>\nWill stress Datarouter with the target load for "
                  << kStimulationDurationSeconds.count() << " seconds.\n";
        return EXIT_FAILURE;
    }

    const auto target_dlt_load_mb_per_sec = std::strtol(argv[1], nullptr, 10);

    if (target_dlt_load_mb_per_sec <= 0)
    {
        std::cout << "Target load must be greater than 0" << std::endl;
    }
#if VERBOSE_LOGGING
    auto& logger = bmw::mw::log::CreateLogger("LOAD");
#else
    bmw::logging::internal::NonVerboseMessage entry;
#endif

    constexpr auto mega_byte_to_byte = 1024 * 1024;
    constexpr auto second_to_milliseconds = 1000;

    const auto number_of_messages_per_chunk =
        ((mega_byte_to_byte * target_dlt_load_mb_per_sec)) /
        (kDltMessagePayloadSizeBytes * (second_to_milliseconds / kChunkDuration.count()));

    std::cout << "Sending " << number_of_messages_per_chunk << " DLT messages with a payload size of "
              << kDltMessagePayloadSizeBytes << " bytes in each " << kChunkDuration.count()
              << " milliseconds interval to simulate a load of " << target_dlt_load_mb_per_sec << " MB/s for "
              << kStimulationDurationSeconds.count() << " seconds." << std::endl;

// Warm up the logging infrastructure, i.e., wait for Datarouter to connect to the client.
#if VERBOSE_LOGGING
    logger.LogFatal() << "Starting with target_dlt_load_mb_per_sec = " << target_dlt_load_mb_per_sec;
#else
    TRACE(entry);
#endif
    std::this_thread::sleep_for(200ms);

#if VERBOSE_LOGGING
    std::vector<char> log_message_buffer(kDltMessagePayloadSizeBytes, 'a');
    bmw::mw::log::LogRawBuffer log_message{log_message_buffer.data(),
                                           static_cast<std::int32_t>(log_message_buffer.size())};
#endif
    const auto start_time = std::chrono::steady_clock::now();
    for (;;)
    {
        const auto elapsed_time = std::chrono::steady_clock::now() - start_time;
        if (elapsed_time >= kStimulationDurationSeconds)
        {
            break;
        }

        const auto chunk_start_time = std::chrono::steady_clock::now();

        for (auto i = 0; i < number_of_messages_per_chunk; ++i)
        {
#if VERBOSE_LOGGING
            logger.LogFatal() << log_message;
#else
            TRACE(entry);
#endif
        }

        std::this_thread::sleep_until(chunk_start_time + kChunkDuration);
    }

    return EXIT_SUCCESS;
}
