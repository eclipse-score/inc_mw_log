// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#include "platform/aas/mw/log/test/stream_capture/stream_capture.h"
#include <gtest/gtest.h>
#include <iostream>

namespace bmw
{
namespace mw
{
namespace log
{
namespace
{

class StreamCaptureFixture : public ::testing::Test
{
};

TEST_F(StreamCaptureFixture, StdoutIsCaptured)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of capturing the standard output");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    StreamCapture stc;
    stc.StartCapturingStdout();
    std::string test_str = "hello world!!";
    std::cout << test_str;
    EXPECT_EQ(stc.FetchCapturedStream(), test_str);
}

TEST_F(StreamCaptureFixture, StderrIsCaptured)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of capturing the standard error");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    StreamCapture stc;
    stc.StartCapturingStderr();
    std::string test_str = "hello world!!";
    std::cerr << test_str;
    EXPECT_EQ(stc.FetchCapturedStream(), test_str);
}

TEST_F(StreamCaptureFixture, ExitIsCalledIfFetchIsCalledBeforeCapturing)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the in-ability of fetching a stream before capturing it.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    StreamCapture stc;
    EXPECT_DEATH(stc.FetchCapturedStream(), "Capture not started");
}

}  // namespace
}  // namespace log
}  // namespace mw
}  // namespace bmw
