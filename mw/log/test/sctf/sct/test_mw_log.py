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

"""Logging sctf test"""
# pylint: disable=global-statement
# pylint: disable=import-error
# pylint: disable=unused-argument
import logging
import pytest
import test_logging.test_log_basic_acceptance as basic_acceptance_test
from sctf import run

LOGGER = logging.getLogger(__name__)

LOGGING_APP_ID = "EXA"

MW_LOG_FILE_NAME = LOGGING_APP_ID + ".dlt"

VALUES_TO_CHECK_MW_LOG_REMOTE = [
    f"{LOGGING_APP_ID} DFLT log info verbose 2 Logging Application DoLogging",
    f"{LOGGING_APP_ID} DFLT log info verbose 2 val_bool True",
    f"{LOGGING_APP_ID} DFLT log debug verbose 8 val_uint8t 123 val_uint16t 1234 val_uint32t 12345 val_uint64t 123456",
    f"{LOGGING_APP_ID} DFLT log warn verbose 8 val_uint8tmax 255 val_uint16tmax 65535 val_uint32tmax 4294967295 val_uint64tmax 18446744073709551615",
    f"{LOGGING_APP_ID} DFLT log fatal verbose 8 val_int8t -34 val_int16t -14576 val_int32t -2147483640 val_int64t -9223372036854775700",
    f"{LOGGING_APP_ID} DFLT log warn verbose 8 val_int8tmax 127 val_int16tmax 32767 val_int32tmax 2147483647 val_int64tmax 9223372036854775807",
    f"{LOGGING_APP_ID} DFLT log debug verbose 8 val_uint8t 123 val_uint16t 1234 val_uint32t 12345 val_uint64t 123456",
    f"{LOGGING_APP_ID} DFLT log warn verbose 8 val_int8tmin -128 val_int16tmin -32768 val_int32tmin -2147483648 val_int64tmin -9223372036854775808",
    f"{LOGGING_APP_ID} DFLT log error verbose 8 val_int8tminplusint8t -94 val_int16tminplusint16t -18192 val_int32tminplusint32t -8 "
        f"val_int64tminplusint64t -108",
    f"{LOGGING_APP_ID} DFLT log fatal verbose 2 val_string Logging",
    f"{LOGGING_APP_ID} DFLT log fatal verbose 2 val_double 93454.6",
    # Hex and bin values are displayed as decimal because of python-dlt limitations. TODO under 
    f"{LOGGING_APP_ID} DFLT log info verbose 8 log_hex_8 10 log_hex_16 9876 log_hex_32 543210987 log_hex_64 654321098765432109",
    f"{LOGGING_APP_ID} DFLT log warn verbose 8 log_bin_8 8 log_bin_16 9012 log_bin_32 3456789012 log_bin_64 3456789012345678901",
    f"{LOGGING_APP_ID} DFLT log fatal verbose 2 log_raw_buffer b\'raw\'",
    f"{LOGGING_APP_ID} DFLT log fatal verbose 2 log_slog2_message slog2_message",
]

VALUES_TO_CHECK_MW_LOG_CONSOLE = [
    f"{LOGGING_APP_ID} DFLT log info verbose 2 Logging Application DoLogging",
    f"{LOGGING_APP_ID} DFLT log info verbose 2 val_bool True",
    f"{LOGGING_APP_ID} DFLT log debug verbose 8 val_uint8t 123 val_uint16t 1234 val_uint32t 12345 val_uint64t 123456",
    f"{LOGGING_APP_ID} DFLT log warn verbose 8 val_uint8tmax 255 val_uint16tmax 65535 val_uint32tmax 4294967295 val_uint64tmax 18446744073709551615",
    f"{LOGGING_APP_ID} DFLT log fatal verbose 8 val_int8t -34 val_int16t -14576 val_int32t -2147483640 val_int64t -9223372036854775700",
    f"{LOGGING_APP_ID} DFLT log warn verbose 8 val_int8tmax 127 val_int16tmax 32767 val_int32tmax 2147483647 val_int64tmax 9223372036854775807",
    f"{LOGGING_APP_ID} DFLT log debug verbose 8 val_uint8t 123 val_uint16t 1234 val_uint32t 12345 val_uint64t 123456",
    f"{LOGGING_APP_ID} DFLT log warn verbose 8 val_int8tmin -128 val_int16tmin -32768 val_int32tmin -2147483648 val_int64tmin -9223372036854775808",
    f"{LOGGING_APP_ID} DFLT log error verbose 8 val_int8tminplusint8t -94 val_int16tminplusint16t -18192 val_int32tminplusint32t -8 "
        f"val_int64tminplusint64t -108",
    f"{LOGGING_APP_ID} DFLT log fatal verbose 2 val_string Logging",
    f"{LOGGING_APP_ID} DFLT log fatal verbose 2 val_double 93454.6",
    # Hex and bin values are displayed as decimal because of python-dlt limitations. TODO under 
    f"{LOGGING_APP_ID} DFLT log info verbose 8 log_hex_8 10 log_hex_16 9876 log_hex_32 543210987 log_hex_64 654321098765432109",
    f"{LOGGING_APP_ID} DFLT log warn verbose 8 log_bin_8 8 log_bin_16 9012 log_bin_32 3456789012 log_bin_64 3456789012345678901",
    f"{LOGGING_APP_ID} DFLT log fatal verbose 2 log_raw_buffer 726177",
    f"{LOGGING_APP_ID} DFLT log fatal verbose 2 log_slog2_message slog2_message",
]

VALUES_TO_CHECK_MW_LOG_FILE = [
    f"{LOGGING_APP_ID} DFLT log info verbose 2 Logging Application DoLogging",
    f"{LOGGING_APP_ID} DFLT log info verbose 2 val_bool True",
    f"{LOGGING_APP_ID} DFLT log debug verbose 8 val_uint8t 123 val_uint16t 1234 val_uint32t 12345 val_uint64t 123456",
    f"{LOGGING_APP_ID} DFLT log warn verbose 8 val_uint8tmax 255 val_uint16tmax 65535 val_uint32tmax 4294967295 val_uint64tmax 18446744073709551615",
    f"{LOGGING_APP_ID} DFLT log fatal verbose 8 val_int8t -34 val_int16t -14576 val_int32t -2147483640 val_int64t -9223372036854775700",
    f"{LOGGING_APP_ID} DFLT log warn verbose 8 val_int8tmax 127 val_int16tmax 32767 val_int32tmax 2147483647 val_int64tmax 9223372036854775807",
    f"{LOGGING_APP_ID} DFLT log debug verbose 8 val_uint8t 123 val_uint16t 1234 val_uint32t 12345 val_uint64t 123456",
    f"{LOGGING_APP_ID} DFLT log warn verbose 8 val_int8tmin -128 val_int16tmin -32768 val_int32tmin -2147483648 val_int64tmin -9223372036854775808",
    f"{LOGGING_APP_ID} DFLT log error verbose 8 val_int8tminplusint8t -94 val_int16tminplusint16t -18192 val_int32tminplusint32t -8 "
        f"val_int64tminplusint64t -108",
    f"{LOGGING_APP_ID} DFLT log fatal verbose 2 val_string Logging",
    f"{LOGGING_APP_ID} DFLT log fatal verbose 2 val_double 93454.6",
    f"{LOGGING_APP_ID} DFLT log info verbose 8 log_hex_8 10 log_hex_16 9876 log_hex_32 543210987 log_hex_64 654321098765432109",
    f"{LOGGING_APP_ID} DFLT log warn verbose 8 log_bin_8 8 log_bin_16 9012 log_bin_32 3456789012 log_bin_64 3456789012345678901",
    f"{LOGGING_APP_ID} DFLT log fatal verbose 2 log_raw_buffer b'raw'",
    f"{LOGGING_APP_ID} DFLT log fatal verbose 2 log_slog2_message slog2_message",
]

@pytest.fixture()
def mw_log_logging_fixture(environment):
    mw_log_logging = basic_acceptance_test.LoggingApp(environment, "mw_log_logging_app")
    mw_log_logging.generate_exec_config()
    yield mw_log_logging

@pytest.mark.metadata(
    description=("mw::log shall implement Specification of Log and Trace for Adaptive Platform"),
    verifies=[1633236, 1633144, 1633238],
    testingTechnique="Interface test",
    derivationTechnique="Analysis of requirements")
def test_mw_log_remote_logging(mw_log_logging_fixture, adaptive_environment_fixture):
    basic_acceptance_test.body_test_case_remote_logging(mw_log_logging_fixture, adaptive_environment_fixture, VALUES_TO_CHECK_MW_LOG_REMOTE, LOGGING_APP_ID)


@pytest.mark.metadata(
    description=("mw::log shall implement Specification of Log and Trace for Adaptive Platform"),
    verifies=[1633236, 1633144, 1633238],
    testingTechnique="Interface test",
    derivationTechnique="Analysis of requirements")
def test_mw_log_console_logging(mw_log_logging_fixture, adaptive_environment_fixture):
    basic_acceptance_test.body_test_case_console_logging(mw_log_logging_fixture, adaptive_environment_fixture, VALUES_TO_CHECK_MW_LOG_CONSOLE, LOGGING_APP_ID)


@pytest.mark.metadata(
    description=("mw::log shall implement Specification of Log and Trace for Adaptive Platform"),
    verifies=[1633236, 1633144, 1633238],
    testingTechnique="Interface test",
    derivationTechnique="Analysis of requirements")
def test_mw_log_file_logging(mw_log_logging_fixture, adaptive_environment_fixture):
    basic_acceptance_test.body_test_case_file_logging(
        mw_log_logging_fixture, adaptive_environment_fixture, VALUES_TO_CHECK_MW_LOG_FILE, LOGGING_APP_ID, MW_LOG_FILE_NAME)


if __name__ == '__main__':
    run(__file__)
