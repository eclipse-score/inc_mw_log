// Copyright 2025 Accenture.
//
// SPDX-License-Identifier: Apache-2.0

use std::ffi::c_void;

#[link(name = "libmw_log_cc")]
unsafe extern "C" {
    /// Create a new logger instance
    pub unsafe fn new_logger(max_level: u8) -> *mut c_void;

    /// Free this logger
    pub unsafe fn free_logger(logger_p: *mut c_void);

    /// Check if this logger is enabled for the given level
    pub unsafe fn logger_enabled(logger_p: *mut c_void, level: u8) -> bool;

    /// Log the specified message with this logger
    pub unsafe fn logger_log(logger_p: *mut c_void, level: u8, msg_ptr: *const u8, msg_len: u64);

    /// Flush this logger
    pub unsafe fn logger_flush();
}
