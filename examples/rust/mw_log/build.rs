// Copyright 2025 Accenture.
//
// SPDX-License-Identifier: Apache-2.0

fn main() {
    println!("cargo::rerun-if-changed=src/include/mw_log.h");
    println!("cargo::rerun-if-changed=src/mw_log.cc");
    println!("cargo::rerun-if-changed=src/ffi.cc");

    cc::Build::new()
        .cpp(true)
        .file("src/include/mw_log.h")
        .file("src/mw_log.cc")
        .file("src/ffi.cc")
        .compile("libmw_log_cc");
}
