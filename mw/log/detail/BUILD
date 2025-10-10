# *******************************************************************************
# Copyright (c) 2025 Contributors to the Eclipse Foundation
#
# See the NOTICE file(s) distributed with this work for additional
# information regarding copyright ownership.
#
# This program and the accompanying materials are made available under the
# terms of the Apache License Version 2.0 which is available at
# https://www.apache.org/licenses/LICENSE-2.0
#
# SPDX-License-Identifier: Apache-2.0
# *******************************************************************************

load("//platform/aas/mw:common_features.bzl", "COMPILER_WARNING_FEATURES")
load("//platform/aas/tools/itf/bazel_gen:itf_gen.bzl", "py_unittest_qnx_test")

cc_library(
    name = "recorder_factory",
    srcs = [
        "recorder_factory.cpp",
    ],
    hdrs = [
        "recorder_factory.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/ara/log/test:__subpackages__",
        "//platform/aas/mw/log:__subpackages__",
        "//platform/aas/tools/kaitai/kaitai_zero_copy_generator:__subpackages__",
    ],
    deps = [
        ":composite_recorder",
        ":empty_recorder",
        ":initialization_reporter",
        "//platform/aas/mw/log:recorder_interface",
        "//platform/aas/mw/log/configuration:configuration_parser",
        "//platform/aas/mw/log/detail/data_router:data_router_backend",
        "//platform/aas/mw/log/detail/file_logging:file_recorder",
        "//platform/aas/mw/log/detail/file_logging:text_recorder",
    ] + select({
        "@platforms//os:linux": [],
        "@platforms//os:qnx": [
            "//platform/aas/mw/log/detail/slog",
        ],
    }),
    alwayslink = True,
)

cc_library(
    name = "logging_identifier",
    srcs = [
        "logging_identifier.cpp",
    ],
    hdrs = [
        "logging_identifier.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log:__subpackages__",
        "//platform/aas/pas/logging:__pkg__",
    ],
    deps = [
        "//third_party/static_reflection_with_serialization_pkg:visitor",
        "@amp",
    ],
)

cc_library(
    name = "types_and_errors",
    srcs = ["error.cpp"],
    hdrs = ["error.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log:__pkg__",
        "//platform/aas/mw/log/configuration:__pkg__",
        "//platform/aas/mw/log/detail/data_router:__pkg__",
        "//platform/aas/mw/log/detail/file_logging:__pkg__",
    ],
    deps = [
        "//platform/aas/lib/result",
    ],
)

cc_library(
    name = "log_entry_deserialize",
    srcs = ["log_entry_deserialize.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//platform/aas/pas/logging:__pkg__",
        "//platform/aas/pas/logging/test:__pkg__",
    ],
    deps = [
        ":logging_identifier",
        "//platform/aas/mw/log:shared_types",
        "//third_party/static_reflection_with_serialization_pkg:visitor",
    ],
)

cc_library(
    name = "log_entry",
    srcs = ["log_entry.cpp"],
    hdrs = ["log_entry.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/ara/log:__pkg__",
        "//platform/aas/mw/log/detail/data_router:__pkg__",
        "//platform/aas/pas/logging:__pkg__",
        "//platform/aas/pas/logging/test:__pkg__",
    ],
    deps = [
        ":logging_identifier",
        "//platform/aas/mw/log:shared_types",
        "//third_party/static_reflection_with_serialization_pkg:visitor",
    ],
)

cc_library(
    name = "backend_interface",
    srcs = ["backend.cpp"],
    hdrs = ["backend.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log/detail/data_router:__pkg__",
        "//platform/aas/mw/log/detail/file_logging:__pkg__",
        "//platform/aas/mw/log/detail/slog:__pkg__",
    ],
    deps = [
        ":log_data_types",
        "//platform/aas/mw/log:recorder",
    ],
)

cc_library(
    name = "log_data_types",
    srcs = [
        "log_record.cpp",
        "verbose_payload.cpp",
    ],
    hdrs = [
        "log_record.h",
        "verbose_payload.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log/detail/data_router:__pkg__",
        "//platform/aas/mw/log/detail/file_logging:__pkg__",
    ],
    deps = [
        ":log_entry",
    ],
)

cc_library(
    name = "statistics_reporter",
    srcs = [
        "istatistics_reporter.cpp",
        "statistics_reporter.cpp",
    ],
    hdrs = [
        "istatistics_reporter.h",
        "statistics_reporter.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log/detail/data_router:__pkg__",
        "//platform/aas/mw/log/detail/file_logging:__pkg__",
    ],
    deps = [
        "//platform/aas/mw/log:recorder",
    ],
)

cc_library(
    name = "dlt_argument_counter",
    srcs = ["dlt_argument_counter.cpp"],
    hdrs = [
        "add_argument_result.h",
        "dlt_argument_counter.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log/detail/data_router:__pkg__",
        "//platform/aas/mw/log/detail/file_logging:__pkg__",
    ],
    deps = [
        "@amp",
    ],
)

cc_library(
    name = "integer_representation",
    srcs = [
        "integer_representation.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log/detail/file_logging:__pkg__",
    ],
)

cc_library(
    name = "dlt_content_formatting",
    srcs = [
        "dlt_format.cpp",
    ],
    hdrs = [
        "dlt_format.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log/detail/data_router:__pkg__",
        "//platform/aas/mw/log/detail/file_logging:__pkg__",
    ],
    deps = [
        ":dlt_argument_counter",
        ":integer_representation",
        ":log_data_types",
        "//platform/aas/lib/bitmanipulation",
        "//platform/aas/lib/memory:string_literal",
        "//platform/aas/mw/log:shared_types",
        "@amp",
    ],
)

cc_library(
    name = "initialization_reporter",
    srcs = [
        "initialization_reporter.cpp",
    ],
    hdrs = [
        "initialization_reporter.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log:__pkg__",
        "//platform/aas/mw/log/configuration:__pkg__",
        "//platform/aas/mw/log/detail/data_router:__pkg__",
        "//platform/aas/mw/log/detail/slog:__pkg__",
    ],
    deps = [
        ":types_and_errors",
    ],
)

cc_library(
    name = "composite_recorder",
    srcs = [
        "composite_recorder.cpp",
    ],
    hdrs = [
        "composite_recorder.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log:__pkg__",
        "//platform/aas/mw/log/detail/data_router:__pkg__",
    ],
    deps = [
        ":initialization_reporter",
        ":types_and_errors",
        "//platform/aas/mw/log:recorder",
    ],
)

cc_library(
    name = "empty_recorder",
    srcs = [
        "empty_recorder.cpp",
    ],
    hdrs = [
        "empty_recorder.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log:__pkg__",
        "//platform/aas/mw/log/detail/data_router:__pkg__",
    ],
    deps = [
        ":log_data_types",
        ":log_entry",
        ":logging_identifier",
        ":types_and_errors",
        "//platform/aas/mw/log:recorder",
        "@amp",
    ],
)

cc_library(
    name = "thread_local_guard",
    srcs = ["thread_local_guard.cpp"],
    hdrs = ["thread_local_guard.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log:__subpackages__",
    ],
)

cc_test(
    name = "unit_test",
    srcs = [
        "circular_allocator_test.cpp",
        "composite_recorder_test.cpp",
        "dlt_argument_counter_test.cpp",
        "dlt_format_test.cpp",
        "empty_recorder_test.cpp",
        "error_test.cpp",
        "log_entry_deserialize_test.cpp",
        "log_record_test.cpp",
        "logging_identifier_test.cpp",
        "recorder_factory_test.cpp",
        "statistics_reporter_test.cpp",
        "thread_local_guard_test.cpp",
        "verbose_payload_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":backend_mock",
        ":log_entry_deserialize",
        ":recorder_factory",
        ":thread_local_guard",
        "//platform/aas/lib/os/mocklib:fcntl_mock",
        "//platform/aas/lib/os/mocklib:unistd_mock",
        "//platform/aas/lib/os/utils/mocklib:path_mock",
        "//platform/aas/mw/log:recorder_mock",
        "//platform/aas/mw/log/configuration:configuration_file_discoverer_mock",
        "//platform/aas/mw/log/configuration:target_config_reader_mock",
        "//platform/aas/mw/log/test/console_logging_environment",
        "//third_party/googletest:main",
    ],
)

cc_library(
    name = "circular_allocator",
    srcs = ["circular_allocator.cpp"],
    hdrs = ["circular_allocator.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log:__subpackages__",
    ],
    deps = ["@amp"],
)

cc_library(
    name = "backend_mock",
    testonly = True,
    srcs = ["backend_mock.cpp"],
    hdrs = ["backend_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//platform/aas/mw/log:__pkg__",
        "//platform/aas/mw/log/detail/data_router:__pkg__",
        "//platform/aas/mw/log/detail/file_logging:__pkg__",
    ],
    deps = [
        "//platform/aas/mw/log/detail:backend_interface",
        "//third_party/googletest",
    ],
)

test_suite(
    name = "unit_tests",
    tests = [
        ":unit_test",
        "//platform/aas/mw/log/detail/data_router:unit_tests",
        "//platform/aas/mw/log/detail/file_logging:unit_tests",
        "//platform/aas/mw/log/detail/slog:unit_tests",
        "//platform/aas/mw/log/detail/wait_free_producer_queue:unit_tests",
        "//platform/aas/mw/log/detail/wait_free_stack:unit_tests",
    ],
    visibility = ["//platform/aas/mw/log:__pkg__"],
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
        "//platform/aas/mw/log/detail/data_router:unit_tests_qnx",
        "//platform/aas/mw/log/detail/file_logging:unit_tests_qnx",
        "//platform/aas/mw/log/detail/slog:unit_tests_qnx",
        "//platform/aas/mw/log/detail/wait_free_producer_queue:unit_tests_qnx",
        "//platform/aas/mw/log/detail/wait_free_stack:unit_tests_qnx",
    ],
    visibility = ["//platform/aas/mw/log:__pkg__"],
)
