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
    name = "legacy_non_verbose_api",
    srcs = ["tracing.cpp"],
    hdrs = ["tracing.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log:__subpackages__",
        "//platform/aas/pas/logging:__pkg__",
    ],
    deps = [
        "//platform/aas/mw/log:frontend",
        "//platform/aas/mw/log/configuration",
        "//platform/aas/mw/log/configuration:nvconfig",
        "//platform/aas/mw/log/detail/data_router/shared_memory:writer",
        "//platform/aas/pas/logging:libtracing_legacy_public_headers",
        "//third_party/static_reflection_with_serialization_pkg:serializer",
        "@amp",
    ],
)

filegroup(
    name = "unit_test_data",
    testonly = True,
    srcs = [
        "test/error-content-json-class-id.json",
        "test/test-class-id.json",
    ],
)

cc_test(
    name = "unit_test",
    srcs = ["tracing_test.cpp"],
    data = [":unit_test_data"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "//platform/aas/lib/os/mocklib:pthread_mock",
        "//platform/aas/lib/os/mocklib:stat_mock",
        "//platform/aas/lib/os/mocklib:stdlib_mock",
        "//platform/aas/lib/os/mocklib:unistd_mock",
        "//platform/aas/lib/os/utils/mocklib:signal_mock",
        "//platform/aas/mw/com/message_passing:mock",
        "//platform/aas/mw/log",
        "//platform/aas/mw/log/detail/data_router/shared_memory:reader",
        "//platform/aas/mw/log/test/console_logging_environment",
        "//platform/aas/pas/logging:filetransfer_message_types",
        "//third_party/googletest:main",
        "@amp//:amp_test_support",
    ],
)

test_suite(
    name = "unit_tests",
    tests = [":unit_test"],
    visibility = ["//platform/aas/mw/log:__pkg__"],
)

py_unittest_qnx_test(
    name = "unit_tests_qnx",
    data_files = [
        ":unit_test_data",
    ],
    test_cases = [
        ":unit_test",
    ],
    visibility = ["//platform/aas/mw/log:__pkg__"],
)
