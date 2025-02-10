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
load("//platform/aas/mw:common_features.bzl", "COMPILER_WARNING_FEATURES")

cc_binary(
    name = "dlt_load_generator",
    srcs = [
        "dlt_load_generator.cpp",
    ],
    defines = ["VERBOSE_LOGGING=0"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        "//platform/aas/mw/log",
    ],
)

# Config for logging app
filegroup(
    name = "logging_cfg",
    srcs = [
        "etc/logging.json",
    ],
    visibility = [
        "//ecu/xpad/xpad-shared/config:__subpackages__",
        "//platform/aas/mw/log/test:__pkg__",
    ],
)

pkg_adaptive_application(
    name = "pkg",
    application_name = "dlt_load_generator",
    bins = [":dlt_load_generator"],
    etc_deps = [],
    etcs = [
        ":logging_cfg",
    ],
    visibility = [
        "//ecu/xpad/xpad-shared/packaging/swe/swfl_platform:__subpackages__",
        "//platform/aas/mw/log/test:__subpackages__",
    ],
)
