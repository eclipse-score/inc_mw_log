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
    name = "configuration",
    srcs = [
        "configuration.cpp",
    ],
    hdrs = [
        "configuration.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log:__subpackages__",
        "//platform/aas/pas/logging:__pkg__",
    ],
    deps = [
        "//platform/aas/mw/log:shared_types",
        "//platform/aas/mw/log/detail:logging_identifier",
    ],
)

cc_library(
    name = "configuration_interface",
    srcs = [
        "iconfiguration_file_discoverer.cpp",
        "itarget_config_reader.cpp",
    ],
    hdrs = [
        "iconfiguration_file_discoverer.h",
        "itarget_config_reader.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log/detail:__pkg__",
    ],
    deps = [
        ":configuration",
        "//platform/aas/lib/result",
    ],
)

cc_library(
    name = "configuration_parser",
    srcs = [
        "configuration_file_discoverer.cpp",
        "target_config_reader.cpp",
    ],
    hdrs = [
        "configuration_file_discoverer.h",
        "target_config_reader.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log/detail:__pkg__",
    ],
    deps = [
        ":configuration",
        ":configuration_interface",
        "//platform/aas/lib/json:json_parser",
        "//platform/aas/lib/memory:split_string_view",
        "//platform/aas/lib/os:stdlib",
        "//platform/aas/lib/os:unistd",
        "//platform/aas/lib/os/utils:path",
        "//platform/aas/mw/log/detail:initialization_reporter",
        "//platform/aas/mw/log/detail:types_and_errors",
    ],
)

cc_library(
    name = "target_config_reader_mock",
    testonly = True,
    srcs = ["target_config_reader_mock.cpp"],
    hdrs = ["target_config_reader_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//platform/aas/mw/log:__pkg__",
        "//platform/aas/mw/log/detail:__pkg__",
        "//platform/aas/mw/log/detail/data_router:__pkg__",
        "//platform/aas/mw/log/detail/slog:__pkg__",
    ],
    deps = [
        ":configuration_interface",
        "//third_party/googletest",
    ],
)

cc_library(
    name = "configuration_file_discoverer_mock",
    testonly = True,
    srcs = ["configuration_file_discoverer_mock.cpp"],
    hdrs = ["configuration_file_discoverer_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//platform/aas/mw/log/detail:__pkg__",
    ],
    deps = [
        ":configuration_interface",
        "//third_party/googletest",
    ],
)

filegroup(
    name = "tests_config_files",
    srcs = [
        "test/data/app_config.json",
        "test/data/ecu_config.json",
        "test/data/error-json-structure.json",
        "test/data/invalid_app_config.json",
        "test/data/syntax_error.json",
        "test/data/wrong-context-config-value.json",
        "test/data/wrong-loglevel-value.json",
    ],
    visibility = ["//platform/aas/mw/log:__pkg__"],
)

cc_test(
    name = "unit_test",
    srcs = [
        "configuration_file_discoverer_test.cpp",
        "configuration_test.cpp",
        "target_config_reader_test.cpp",
    ],
    data = [
        ":tests_config_files",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":configuration_file_discoverer_mock",
        ":configuration_parser",
        ":target_config_reader_mock",
        "//platform/aas/lib/os/mocklib:fcntl_mock",
        "//platform/aas/lib/os/mocklib:stdlib_mock",
        "//platform/aas/lib/os/mocklib:unistd_mock",
        "//platform/aas/lib/os/utils/mocklib:path_mock",
        "//platform/aas/mw/log/test/console_logging_environment",
        "//third_party/googletest:main",
    ],
)

cc_library(
    name = "nvconfig",
    srcs = [
        "nvconfig.cpp",
    ],
    hdrs = [
        "nvconfig.h",
        "nvmsgdescriptor.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log:__subpackages__",
        "//platform/aas/pas/logging:__subpackages__",
    ],
    deps = [
        "//platform/aas/lib/json:json_parser",
        "//platform/aas/mw/log/detail:logging_identifier",
        "@amp",
    ],
)

cc_test(
    name = "nvconfig_test",
    srcs = [
        "nvconfig_test.cpp",
    ],
    data = [
        "test/data/empty-class-id.json",
        "test/data/empty-json-class-id.json",
        "test/data/error-content-1-json-class-id.json",
        "test/data/error-content-2-json-class-id.json",
        "test/data/error-content-3-json-class-id.json",
        "test/data/error-content-wrong-id-value.json",
        "test/data/error-parse-1-json-class-id.json",
        "test/data/error-parse-empty-json-object.json",
        "test/data/second-test-class-id.json",
        "test/data/test-class-id.json",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":nvconfig",
        "//third_party/googletest:main",
    ],
)

filegroup(
    name = "nvconfig_test_config_files",
    srcs = [
        "test/data/empty-class-id.json",
        "test/data/empty-json-class-id.json",
        "test/data/error-content-1-json-class-id.json",
        "test/data/error-content-2-json-class-id.json",
        "test/data/error-content-3-json-class-id.json",
        "test/data/error-content-wrong-id-value.json",
        "test/data/error-parse-1-json-class-id.json",
        "test/data/error-parse-empty-json-object.json",
        "test/data/second-test-class-id.json",
        "test/data/test-class-id.json",
    ],
    visibility = ["//platform/aas/mw/log:__pkg__"],
)

test_suite(
    name = "unit_tests",
    tests = [
        ":nvconfig_test",
        ":unit_test",
    ],
    visibility = ["//platform/aas/mw/log:__pkg__"],
)

py_unittest_qnx_test(
    name = "unit_tests_qnx",
    data_files = [
        ":tests_config_files",
        ":nvconfig_test_config_files",
    ],
    test_cases = [
        ":unit_test",
        ":nvconfig_test",
    ],
    visibility = ["//platform/aas/mw:__subpackages__"],
)
