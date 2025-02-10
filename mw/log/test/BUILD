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

load("//platform/aas/bazel/packaging:adaptive_application.bzl", "pkg_adaptive_application")
load("//platform/aas/tools/itf/bazel_gen:itf_gen.bzl", "py_unittest_qnx_test")

_compiler_warning_features = [
    "additional_warnings",
    "strict_warnings",
    "treat_warnings_as_errors",
]

pkg_adaptive_application(
    name = "mw_log_logging_app",
    testonly = True,
    bins = ["//platform/aas/mw/log/test/logging_app:mw_log_logging_app"],
    etc_deps = [],
    etcs = [
        "//platform/aas/mw/log/test/logging_app:logging_cfg",
    ],
    visibility = ["//platform/aas/mw/log/test:__subpackages__"],
)

# Since //common shall not depend on platform (Since it is in another stand alone repo
# "static_reflection_with_serialization"), and because this new repo is not ready to build
# and run the unit tests on QNX, we have to integrate the qnx unit tests here. Kindly check 
# visitor unit tests.
filegroup(
    name = "visitor_unit_tests_fg",
    testonly = True,
    srcs = [":visitorUT"],
)

cc_test(
    name = "visitorUT",
    srcs = [
        "@static_reflection_with_serialization//common/visitor:test/ut/test_detail.cpp",
        "@static_reflection_with_serialization//common/visitor:test/ut/test_struct_visitor.cpp",
        "@static_reflection_with_serialization//common/visitor:test/ut/test_visitor.cpp",
    ],
    features = _compiler_warning_features,
    tags = ["unit"],
    deps = [
        "//third_party/c++stdlib",
        "@googletest//:gtest_main",
        "@static_reflection_with_serialization//common/visitor:visitor_target",
    ],
)

# serializer unit tests.
filegroup(
    name = "serializer_unit_tests_fg",
    testonly = True,
    srcs = [
        ":VisitorTypeTraitsUT",
        ":serializerUT",
        ":sizeVisitorUT",
    ],
)

cc_test(
    name = "serializerUT",
    srcs = [
        "@static_reflection_with_serialization//common/serialization:test/ut/test_serializer_visitor.cpp",
        "@static_reflection_with_serialization//common/serialization:test/ut/test_skip_deserialize.cpp",
        "@static_reflection_with_serialization//common/serialization:test/ut/visitor_test_types.h",
    ],
    features = _compiler_warning_features + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@static_reflection_with_serialization//common/serialization:serializer_target",
    ],
)

cc_test(
    name = "sizeVisitorUT",
    srcs = [
        "@static_reflection_with_serialization//common/serialization:test/ut/test_size_visitor.cpp",
        "@static_reflection_with_serialization//common/serialization:test/ut/visitor_test_types.h",
    ],
    features = _compiler_warning_features + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@static_reflection_with_serialization//common/serialization:serializer_target",
    ],
)

cc_test(
    name = "VisitorTypeTraitsUT",
    srcs = [
        "@static_reflection_with_serialization//common/serialization:test/ut/test_visitor_type_traits.cpp",
        "@static_reflection_with_serialization//common/serialization:test/ut/visitor_test_types.h",
    ],
    features = _compiler_warning_features + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        "@googletest//:gtest_main",
        "@static_reflection_with_serialization//common/serialization:serializer_target",
    ],
)

py_unittest_qnx_test(
    name = "serializer_unit_tests_qnx",
    test_cases = [
        ":visitor_unit_tests_fg",
        ":serializer_unit_tests_fg",
    ],
    visibility = ["//platform/aas:__subpackages__"],
)
