// Copyright 2025 Accenture.
//
// SPDX-License-Identifier: Apache-2.0

//! Rust-to-C++ logging example
//!
//! This library contains example code showing how a Rust logger can forward logging calls
//! to a logger implementation in C++.

use std::{
    cell::UnsafeCell,
    ffi::c_void,
    io::{Cursor, Write as _},
};

use log::{LevelFilter, Log};

mod ffi;

/// Initialize logging with the given level filter
pub fn init_logging(level: LevelFilter) {
    let logger = MwLogger::new(level);
    log::set_max_level(level);
    log::set_boxed_logger(Box::new(logger)).expect("failed to set logger");
}

/// Size of the buffer into which we write log messages
const LOG_MSG_BUFFER_SIZE: usize = 1024;

/// Logger implementation forwarding to `mw_log` in C++
struct MwLogger {
    /// Pointer to initialized logger instance in C++
    logger: *mut c_void,
}

// The `log::Log` trait is a subtrait of `Send` and `Sync`.
// The mutable pointer does not implement `Send` and `Sync`.
// To be able to implement `log::Log`, we need to (unsafely) implement `Send` and `Sync`.
// This is safe only if all calls we make through the FFI are thread-safe.
// At the moment, the implementation is thread-safe in the sense of not leading to memory corruption,
// but messages can get interleaved, which we do not want.
// We can prevent this on either side of the FFI boundary, e.g.:
// - Writing messages to a channel on the Rust side and forward with a defined order in a separate thread.
// - Rely on a thread-safe mechanism of the underlying C++ implementation.
// - ...
// SAFETY: All calls which we do using the pointer are thread-safe from a memory perspective.
unsafe impl Send for MwLogger {}
// SAFETY: All calls which we do using the pointer are thread-safe from a memory perspective.
unsafe impl Sync for MwLogger {}

impl MwLogger {
    /// Create a new instance.
    fn new(level: LevelFilter) -> Self {
        let level = level as u8;
        let logger = unsafe { ffi::new_logger(level) };
        Self { logger }
    }
}

impl Drop for MwLogger {
    fn drop(&mut self) {
        if !self.logger.is_null() && self.logger.is_aligned() {
            // SAFETY: `self.logger` is a valid pointer to an initialized logger on the C++ side
            unsafe { ffi::free_logger(self.logger) };
        }
    }
}

impl Log for MwLogger {
    fn enabled(&self, metadata: &log::Metadata) -> bool {
        let level = metadata.level() as u8;
        // SAFETY: `self.logger` is a valid pointer to an initialized logger on the C++ side
        unsafe { ffi::logger_enabled(self.logger, level) }
    }

    fn log(&self, record: &log::Record) {
        if !self.enabled(record.metadata()) {
            return;
        }

        // Create thread-local buffer
        // Despite its name, the `UnsafeCell` is not inherently unsafe.
        // We just need `unsafe` to access the data mutably.
        // If you prefer a buffer without `unsafe`, `RefCell` is an option,
        // although it introduces a very small runtime overhead.
        thread_local! {
            static BUFFER: UnsafeCell<[u8; LOG_MSG_BUFFER_SIZE]> = const {UnsafeCell::new([0; LOG_MSG_BUFFER_SIZE])};
        }

        // Access thread-local buffer
        BUFFER.with(|buffer| {
            // SAFETY: During the complete lifetime of this borrow, there is no other borrow alive
            // (mutable or immutable).
            let buffer = unsafe { &mut *buffer.get() };

            // Write log to the buffer
            let mut cursor = Cursor::new(&mut buffer[..]);
            if write!(cursor, "{}", record.args()).is_err() {
                // Skip logging if we did not write successfully
                return;
            };
            let msg_ptr = cursor.get_ref().as_ptr();
            let msg_len = cursor.position();

            let level = record.level() as u8;

            // SAFETY:
            // - `self.logger` is a valid pointer to an initialized logger on the C++ side
            // - `msg_ptr` is created from an initialized buffer within the cursor
            // - `msg_len` is shorter or equal to the size of the buffer behind `msg_ptr`,
            //   all bytes until `msg_len` are written to
            unsafe { ffi::logger_log(self.logger, level, msg_ptr, msg_len) };
        })
    }

    fn flush(&self) {
        unsafe { ffi::logger_flush() };
    }
}
