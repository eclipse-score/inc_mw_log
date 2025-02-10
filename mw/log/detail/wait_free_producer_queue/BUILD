# *******************************************************************************>
# Copyright (c) 2024 Contributors to the Eclipse Foundation
# See the NOTICE file(s) distributed with this work for additional
# information regarding copyright ownership.
# This program and the accompanying materials are made available under the
# terms of the Apache License Version 2.0 which is available at
# https://www.apache.org/licenses/LICENSE-2.0
# SPDX-License-Identifier: Apache-2.0 #
# *******************************************************************************


# 
load("//platform/aas/mw:common_features.bzl", "COMPILER_WARNING_FEATURES")
load("//platform/aas/tools/itf/bazel_gen:itf_gen.bzl", "py_unittest_qnx_test")

cc_library(
    name = "wait_free_producer_queue",
    srcs = [
        "alternating_control_block.cpp",
        "alternating_reader.cpp",
        "linear_control_block.cpp",
        "linear_reader.cpp",
        "wait_free_alternating_writer.cpp",
        "wait_free_linear_writer.cpp",
    ],
    hdrs = [
        "alternating_control_block.h",
        "alternating_reader.h",
        "linear_control_block.h",
        "linear_reader.h",
        "wait_free_alternating_writer.h",
        "wait_free_linear_writer.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//platform/aas/mw/log/detail/data_router:__subpackages__"],
    deps = [
        "@amp",
    ],
)

cc_test(
    name = "unit_tests",
    srcs = [
        "alternating_control_block_test.cpp",
        "linear_control_block_test.cpp",
        "linear_reader_test.cpp",
        "wait_free_alternating_writer_test.cpp",
        "wait_free_linear_writer_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    linkopts = select({
        "@platforms//os:linux": ["-latomic"],  # needed for spp-host-clang
        "//conditions:default": [],
    }),
    tags = ["unit"],
    visibility = ["//platform/aas/mw/log/detail:__pkg__"],
    deps = [
        ":wait_free_producer_queue",
        "//platform/aas/mw/log/test/console_logging_environment",
        "//third_party/googletest:main",
        "//third_party/libatomic",
    ],
)

py_unittest_qnx_test(
    name = "pkg_unit_tests_qnx",
    test_cases = [
        ":unit_tests",
    ],
)

py_unittest_qnx_test(
    name = "unit_tests_qnx",
    tags = ["manual"],
    test_suites = [
        ":pkg_unit_tests_qnx",
    ],
    visibility = ["//platform/aas/mw/log/detail:__pkg__"],
)
