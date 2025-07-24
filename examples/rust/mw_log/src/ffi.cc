// Copyright 2025 Accenture.
//
// SPDX-License-Identifier: Apache-2.0

#include "include/mw_log.h"

// FFI API reachable from Rust
extern "C"
{

    // Create a new logger instance
    void *new_logger(uint8_t max_level)
    {
        Logger *logger = new Logger(max_level);
        return (void *)logger;
    }

    // Free this logger
    void free_logger(void *logger_p)
    {
        if (logger_p != nullptr)
        {
            Logger *logger = (Logger *)logger_p;
            delete logger;
        }
    }

    // Check if this logger is enabled for the given metadata
    //
    // Has to be thread-safe
    bool logger_enabled(void *logger_p, uint8_t level)
    {
        if (logger_p != nullptr)
        {
            Logger *logger = (Logger *)logger_p;
            return logger->enabled(level);
        }
        else
        {
            return false;
        }
    }

    // Log the specified message with this logger
    //
    // Has to be thread-safe
    void logger_log(void *logger_p, uint8_t level, const unsigned char *msg_ptr, uint64_t msg_len)
    {
        if (logger_p != nullptr)
        {

            Logger *logger = (Logger *)logger_p;
            logger->log(level, msg_ptr, msg_len);
        }
    }

    // Flush this logger
    //
    // Has to be thread-safe
    void logger_flush(void *logger_p)
    {
        if (logger_p != nullptr)
        {
            Logger *logger = (Logger *)logger_p;
            logger->flush();
        }
    }
}
