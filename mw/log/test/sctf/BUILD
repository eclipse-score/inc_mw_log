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
load("//platform/aas/tools/sctf/bazel_gen:sctf_gen.bzl", "py_sctf_test")

py_sctf_test(
    name = "test_mw_log",
    srcs = [
        "sct/__init__.py",
        "sct/test_mw_log.py",
    ],
    data = [
        "//platform/aas/mw/log/test:mw_log_logging_app-pkg.tar",
    ],
    flaky = False,
    module_root = "platform/aas/mw/log/test/sctf/sct",
    deps = [
        "//platform/aas/tools/logging:test_logging",
        "//platform/aas/tools/perf_report",
    ],
)
