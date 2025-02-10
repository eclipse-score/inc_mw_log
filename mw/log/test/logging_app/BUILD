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

# Mw Log Logging Adaptive Application
cc_binary(
    name = "mw_log_logging_app",
    testonly = True,
    srcs = [
        "main.cpp",
    ],
    defines = ["USE_MIDDLEWARE_LOGGING=1"],
    features = COMPILER_WARNING_FEATURES,
    visibility = [
        "//ecu/xpad/xpad-shared/config:__subpackages__",
        "//platform/aas/mw/log/test:__pkg__",
    ],
    deps = [
        "//platform/aas/mw/log:frontend",
        "//platform/aas/mw/log/detail:recorder_factory",
        "//platform/aas/test/pas/logging/utils:logging_app_test_template",
    ],
)
