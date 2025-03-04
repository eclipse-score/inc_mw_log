// Copyright 2025 Accenture.
//
// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "include/mw_log.h"

// Check if this logger is enabled for the given level
bool Logger::enabled(uint8_t level)
{
    return level <= this->max_level;
}

// Log the message
void Logger::log(uint8_t level, const unsigned char *msg_ptr, uint64_t msg_len)
{

    if (msg_ptr != nullptr)
    {
        write_level(level);
        std::cout.write(reinterpret_cast<const char *>(msg_ptr), msg_len) << std::endl;
    }
}

// Flush the logger
void Logger::flush()
{
    std::cout.flush();
}

// Write the log level
void write_level(uint8_t level)
{
    switch (level)
    {
    case 1:
        std::cout << "ERROR: ";
        break;
    case 2:
        std::cout << "WARN : ";
        break;
    case 3:
        std::cout << "INFO : ";
        break;
    case 4:
        std::cout << "DEBUG: ";
        break;
    case 5:
    default:
        std::cout << "TRACE: ";
    }
}