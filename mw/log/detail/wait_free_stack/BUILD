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
    name = "wait_free_stack",
    hdrs = [
        "wait_free_stack.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/ara/log:__subpackages__",
        "//platform/aas/mw/log:__subpackages__",
    ],
    deps = ["@amp"],
)

cc_test(
    name = "unit_test",
    srcs = ["wait_free_stack_test.cpp"],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    linkopts = select({
        "@platforms//os:linux": ["-latomic"],  # needed for spp-host-clang
        "//conditions:default": [],
    }),
    tags = ["unit"],
    deps = [
        ":wait_free_stack",
        "//third_party/googletest:main",
        "//third_party/libatomic",
    ],
)

test_suite(
    name = "unit_tests",
    tests = ["unit_test"],
    visibility = ["//platform/aas/mw/log/detail:__pkg__"],
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
    visibility = ["//platform/aas/mw/log/detail:__pkg__"],
)
