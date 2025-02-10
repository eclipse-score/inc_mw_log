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
    name = "slog_backend",
    srcs = [
        "slog_backend.cpp",
    ],
    hdrs = [
        "slog_backend.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//platform/aas/mw/log:__subpackages__"],
    deps = [
        "//platform/aas/lib/os/qnx:slog2",
        "//platform/aas/mw/log/detail:backend_interface",
        "//platform/aas/mw/log/detail:circular_allocator",
        "//platform/aas/mw/log/detail:initialization_reporter",
    ],
)

cc_library(
    name = "slog",
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    target_compatible_with = ["@platforms//os:qnx"],
    visibility = ["//platform/aas/mw/log:__subpackages__"],
    deps = [":slog_backend"],
)

py_unittest_qnx_test(
    name = "unit_tests_qnx",
    test_cases = [
        ":slog_backend_test",
    ],
    visibility = ["//platform/aas/mw/log/detail:__pkg__"],
)

test_suite(
    name = "unit_tests",
    tests = [],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "slog_backend_test",
    srcs = [
        "slog_backend_test.cpp",
    ],
    features = [
        "aborts_upon_exception",
    ],
    tags = [
        "manual",
        "unit",
    ],
    deps = [
        "//platform/aas/mw/log/configuration",
        "//third_party/googletest",
        "//third_party/googletest:main",
        "@amp//:amp_test_support",
    ] + select({
        "@platforms//os:linux": [],
        "@platforms//os:qnx": [
            ":slog",
            "//platform/aas/lib/os/mocklib/qnx:slog2_mock",
        ],
    }),
)
