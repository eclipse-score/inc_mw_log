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
    name = "file_recorder",
    srcs = [
        "dlt_message_builder.cpp",
        "dlt_message_builder.h",
        "dlt_message_builder_types.h",
        "file_recorder.cpp",
    ],
    hdrs = [
        "file_recorder.h",
        "svp_time.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log/detail:__pkg__",
    ],
    deps = [
        ":file_output_backend",
        ":message_builder_interface",
        "//platform/aas/lib/os/utils:high_resolution_steady_clock",
        "//platform/aas/mw/log:recorder",
        "//platform/aas/mw/log:shared_types",
        "//platform/aas/mw/log/configuration",
        "//platform/aas/mw/log/detail:backend_interface",
        "//platform/aas/mw/log/detail:dlt_content_formatting",
        "//platform/aas/mw/log/detail:statistics_reporter",
        "//third_party/static_reflection_with_serialization_pkg:serializer",
        "@amp",
    ],
)

cc_library(
    name = "text_recorder",
    srcs = [
        "text_message_builder.cpp",
        "text_message_builder.h",
        "text_recorder.cpp",
    ],
    hdrs = [
        "text_recorder.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log/detail:__pkg__",
    ],
    deps = [
        ":file_output_backend",
        ":message_builder_interface",
        ":text_content_formatting",
        "//platform/aas/lib/os/utils:high_resolution_steady_clock",
        "//platform/aas/mw/log:recorder",
        "//platform/aas/mw/log:shared_types",
        "//platform/aas/mw/log/configuration",
        "//platform/aas/mw/log/detail:backend_interface",
        "//platform/aas/mw/log/detail:dlt_argument_counter",
        "@amp",
    ],
)

cc_library(
    name = "text_content_formatting",
    srcs = [
        "text_format.cpp",
    ],
    hdrs = [
        "text_format.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    deps = [
        "//platform/aas/lib/memory:string_literal",
        "//platform/aas/mw/log:shared_types",
        "//platform/aas/mw/log/detail:dlt_argument_counter",
        "//platform/aas/mw/log/detail:integer_representation",
        "//platform/aas/mw/log/detail:log_data_types",
        "@amp",
    ],
)

cc_library(
    name = "non_blocking_writer",
    srcs = ["non_blocking_writer.cpp"],
    hdrs = ["non_blocking_writer.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log/detail:__pkg__",
    ],
    deps = [
        "//platform/aas/lib/os:unistd",
        "//platform/aas/mw/log/detail:types_and_errors",
        "@amp",
    ],
)

cc_library(
    name = "message_builder_interface",
    srcs = [
        "imessage_builder.cpp",
    ],
    hdrs = [
        "imessage_builder.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log:__pkg__",
    ],
    deps = [
        "//platform/aas/mw/log/detail:log_data_types",
    ],
)

cc_library(
    name = "file_output_backend",
    srcs = [
        "file_output_backend.cpp",
        "slot_drainer.cpp",
        "slot_drainer.h",
    ],
    hdrs = [
        "file_output_backend.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//platform/aas/mw/log:__pkg__"],
    deps = [
        ":message_builder_interface",
        ":non_blocking_writer",
        "//platform/aas/lib/os:fcntl",
        "//platform/aas/mw/log/detail:backend_interface",
        "//platform/aas/mw/log/detail:circular_allocator",
        "@amp",
    ],
)

cc_test(
    name = "unit_test",
    srcs = [
        "dlt_message_builder_test.cpp",
        "file_output_backend_test.cpp",
        "file_recorder_test.cpp",
        "slot_drainer_test.cpp",
        "text_format_test.cpp",
        "text_message_builder_test.cpp",
        "text_recorder_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":file_output_backend",
        ":file_output_backend_mocks",
        ":file_recorder",
        ":text_recorder",
        "//platform/aas/lib/os/mocklib:fcntl_mock",
        "//platform/aas/lib/os/mocklib:unistd_mock",
        "//platform/aas/lib/os/utils/mocklib:path_mock",
        "//platform/aas/mw/log/detail:backend_mock",
        "//platform/aas/mw/log/detail:circular_allocator",
        "//platform/aas/mw/log/test/console_logging_environment",
        "//third_party/googletest:main",
    ],
)

cc_library(
    name = "file_output_backend_mocks",
    testonly = True,
    hdrs = ["mock/message_builder_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":non_blocking_writer",
        "//third_party/googletest",
        "@amp",
    ],
)

test_suite(
    name = "unit_tests",
    tests = [
        ":non_blocking_writer_test",
        ":unit_test",
    ],
    visibility = ["//platform/aas/mw/log:__pkg__"],
)

cc_test(
    name = "non_blocking_writer_test",
    srcs = ["non_blocking_writer_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":non_blocking_writer",
        "//platform/aas/lib/os/mocklib:unistd_mock",
        "//platform/aas/mw/log/test/console_logging_environment",
        "//third_party/googletest:main",
    ],
)

py_unittest_qnx_test(
    name = "pkg_unit_tests_qnx",
    test_cases = [
        ":unit_test",
    ],
)

py_unittest_qnx_test(
    name = "unit_tests_qnx",
    tags = ["manual"],
    test_suites = [
        ":pkg_unit_tests_qnx",
    ],
    visibility = ["//platform/aas/mw/log:__subpackages__"],
)
