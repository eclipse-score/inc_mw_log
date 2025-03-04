// Copyright 2025 Accenture.
//
// SPDX-License-Identifier: Apache-2.0

#include <cstdint>

// Logger class which Rust logging forwards to
class Logger
{
public:
    Logger(uint8_t max_level) : max_level(max_level) {}

    // All methods called by the Rust logger implementation have to be thread-safe

    bool enabled(uint8_t level);
    void log(uint8_t level, const unsigned char *msg_ptr, uintptr_t msg_len);
    void flush();

private:
    uint8_t max_level;
};

void write_level(uint8_t level);