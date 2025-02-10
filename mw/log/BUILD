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
    name = "log",
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = ["//visibility:public"],
    deps = [
        ":frontend",
        "//platform/aas/mw/log/detail:recorder_factory",
    ],
)

cc_library(
    name = "recorder_interface",
    srcs = ["irecorder_factory.cpp"],
    hdrs = ["irecorder_factory.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log/detail:__subpackages__",
    ],
    deps = [":recorder"],
)

# TODO: make use of implementation_deps and private header files
cc_library(
    name = "frontend",
    srcs = [
        "log_stream_factory.cpp",
        "logger.cpp",
        "logger_container.cpp",
        "logging.cpp",
        "runtime.cpp",
    ],
    hdrs = [
        "log_stream_factory.h",
        "logger.h",
        "logger_container.h",
        "logging.h",
        "runtime.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/ara/log:__subpackages__",
        "//platform/aas/lib/memory/shared:__subpackages__",  # Needed to break cyclic dependency ()
        "//platform/aas/mw/log/detail:__subpackages__",
        "//platform/aas/mw/log/legacy_non_verbose_api:__pkg__",
        "//platform/aas/mw/log/test:__subpackages__",
        "//platform/aas/mw/log/test/sctf_example:__subpackages__",
    ],
    deps = [
        ":log_stream",
        ":recorder",
        ":recorder_interface",
        ":shared_types",
        "//platform/aas/lib/result",
        "//platform/aas/mw/log/detail:circular_allocator",
        "//platform/aas/mw/log/detail:thread_local_guard",
        "//platform/aas/mw/log/detail/wait_free_stack",
        "@amp",
    ] + select({
        "@platforms//os:qnx": [
            "//platform/aas/lib/os/qnx:channel",
            "//platform/aas/lib/os/qnx:dispatch",
            "//platform/aas/lib/os/qnx:iofunc",
        ],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "shared_types",
    srcs = [
        "log_common.cpp",
        "log_level.cpp",
        "slot_handle.cpp",
    ],
    hdrs = [
        "log_common.h",
        "log_level.h",
        "log_mode.h",
        "log_types.h",
        "slot_handle.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/ara/log:__subpackages__",
        "//platform/aas/mw/log/configuration:__pkg__",
        "//platform/aas/mw/log/detail:__pkg__",
        "//platform/aas/mw/log/detail/data_router:__pkg__",
        "//platform/aas/mw/log/detail/file_logging:__pkg__",
        "//platform/aas/mw/log/detail/slog:__pkg__",
        "//platform/aas/test/pas/logging/utils:__pkg__",
    ],
    deps = ["@amp"],
)

cc_library(
    name = "recorder",
    srcs = ["recorder.cpp"],
    hdrs = ["recorder.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/mw/log/detail:__pkg__",
        "//platform/aas/mw/log/detail/data_router:__pkg__",
        "//platform/aas/mw/log/detail/file_logging:__pkg__",
        "//platform/aas/mw/log/detail/slog:__pkg__",
    ],
    deps = [
        ":shared_types",
        "@amp",
    ],
)

cc_library(
    name = "recorder_mock",
    testonly = True,
    hdrs = ["recorder_mock.h"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//application/adp/aas/logging:__pkg__",
        "//ecu/domains/cdc/cdc_wasm:__subpackages__",
        "//platform/aas/ara/AasDiagProvider:__subpackages__",
        "//platform/aas/ara/log/test:__pkg__",
        "//platform/aas/lib/os/test:__pkg__",
        "//platform/aas/lib/result:__pkg__",
        "//platform/aas/mw/log/detail:__pkg__",
        "//platform/aas/os/secpol/secpollog:__pkg__",
        "//platform/aas/pas/monitoring/hmon/code/app:__pkg__",
        "//platform/aas/sysfunc/ConfigDaemon/code/plugins/calibration:__subpackages__",
        "//platform/aas/sysfunc/ISOCMinion/code:__subpackages__",
        "//platform/aas/sysfunc/SoftwareIdProvider:__subpackages__",
        "//platform/aas/sysfunc/SoftwareUpdate:__subpackages__",
        "//platform/aas/sysfunc/common:__subpackages__",
    ],
    deps = [
        ":recorder",
        "//third_party/googletest",
    ],
)

cc_library(
    name = "log_stream",
    srcs = ["log_stream.cpp"],
    hdrs = ["log_stream.h"],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "//platform/aas/lib/os:__pkg__",
        "//platform/aas/lib/result:__pkg__",
        "//platform/aas/mw/log/detail:__pkg__",
        "//platform/aas/mw/log/test/my_custom_lib:__pkg__",
        "//platform/aas/mw/log/test/sample_ara_core_logging:__pkg__",
    ],
    deps = [
        ":recorder",
        ":shared_types",
        "//platform/aas/mw/log/detail:logging_identifier",
        "//platform/aas/mw/log/detail:thread_local_guard",
        "@amp",
    ],
)

TEST_DEPS = [
    ":frontend",
    ":log_stream",
    ":recorder_mock",
    "//platform/aas/mw/log/detail:backend_mock",
    "//platform/aas/mw/log/detail:recorder_factory",
    "//platform/aas/mw/log/test/my_custom_lib:my_custom_type_mw_log",
    "//platform/aas/mw/log/test/sample_ara_core_logging:sample_ara_core_logging",
    "//third_party/googletest:main",
]

cc_test(
    name = "unit_test",
    srcs = [
        ":log_level_test.cpp",
        ":log_stream_test.cpp",
        ":log_types_test.cpp",
        ":logger_container_test.cpp",
        ":logger_test.cpp",
        ":logging_test.cpp",
        ":runtime_test.cpp",
        ":slot_handle_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = TEST_DEPS,
)

cc_test(
    name = "runtime_test_without_pointer_initialization",
    srcs = [
        "runtime_test_without_pointer_initialization.cpp",
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = TEST_DEPS,
)

test_suite(
    name = "unit_tests",
    tests = [
        ":runtime_test_without_pointer_initialization",
        ":unit_test",
        "//platform/aas/mw/log/configuration:unit_tests",
        "//platform/aas/mw/log/detail:unit_tests",
        "//platform/aas/mw/log/legacy_non_verbose_api:unit_tests",
    ],
    visibility = ["//platform/aas/mw:__pkg__"],
)

py_unittest_qnx_test(
    name = "pkg_unit_tests_qnx",
    data_files = [
        "//platform/aas/mw/log/configuration:tests_config_files",
    ],
    test_cases = [
        ":unit_test",
        ":runtime_test_without_pointer_initialization",
    ],
    visibility = ["//platform/aas:__subpackages__"],
)

py_unittest_qnx_test(
    name = "unit_tests_qnx",
    tags = ["manual"],
    test_suites = [
        ":pkg_unit_tests_qnx",
        "//platform/aas/mw/log/configuration:unit_tests_qnx",
        "//platform/aas/mw/log/detail:unit_tests_qnx",
        "//platform/aas/mw/log/legacy_non_verbose_api:unit_tests_qnx",
        "//platform/aas/mw/log/test:serializer_unit_tests_qnx",
    ],
    visibility = ["//platform/aas/mw:__subpackages__"],
)
