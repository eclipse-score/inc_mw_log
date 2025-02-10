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

cc_library(
    name = "stream_capture",
    testonly = True,
    srcs = [
        "stream_capture.cpp",
    ],
    hdrs = [
        "stream_capture.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["//platform/aas:__subpackages__"],
)

cc_test(
    name = "unit_test",
    srcs = [
        "stream_capture_test.cpp",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["unit"],
    deps = [
        ":stream_capture",
        "//third_party/googletest:main",
    ],
)
