/********************************************************************************
* Copyright (c) 2025 Contributors to the Eclipse Foundation
*
* See the NOTICE file(s) distributed with this work for additional
* information regarding copyright ownership.
*
* This program and the accompanying materials are made available under the
* terms of the Apache License Version 2.0 which is available at
* https://www.apache.org/licenses/LICENSE-2.0
*
* SPDX-License-Identifier: Apache-2.0
********************************************************************************/


#include "platform/aas/mw/log/test/stream_capture/stream_capture.h"
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <string>

namespace bmw
{
namespace mw
{
namespace log
{
void StreamCapture::StartCapturingStream(FILE* stream)
{
    old_stream_ = dup(fileno(stream));
    if (old_stream_ == -1)
    {
        std::cerr << "dup call failed" << std::endl;
        exit(-1);
    }

    file_name_ = "test_" + std::to_string(rand()) + ".txt";
    std::FILE* fileW = std::fopen(file_name_.c_str(), "w");
    if (!fileW)
    {
        std::cerr << "Failed to open file" << std::endl;
        exit(-1);
    }

    fflush(stream);
    if (dup2(fileno(fileW), fileno(stream)) == -1)
    {
        std::cerr << "dup call failed" << std::endl;
        exit(-1);
    }
    capturing_ = true;
    fclose(fileW);
}

std::string StreamCapture::FetchCapturedStream()
{
    if (!capturing_)
    {
        std::cerr << "Capture not started" << std::endl;
        exit(-1);
    }

    auto stream = getStreamFile();
    fflush(stream);
    if (dup2(old_stream_, fileno(stream) == -1))
    {
        std::cerr << "dup call failed" << std::endl;
        exit(-1);
    }
    close(old_stream_);
    std::FILE* file = std::fopen(file_name_.c_str(), "r");

    if (!file)
    {
        std::cerr << "Failed to open file" << std::endl;
        exit(-1);
    }

    char ch = static_cast<char>(getc(file));
    std::string str;
    while (ch != EOF)
    {
        str.push_back(ch);
        ch = static_cast<char>(getc(file));
    }

    fclose(file);
    std::remove(file_name_.c_str());
    return str;
}

void StreamCapture::StartCapturingStdout()
{
    StartCapturingStream(stdout);
    stream_type_ = StreamType::STDOUT;
}

void StreamCapture::StartCapturingStderr()
{
    StartCapturingStream(stderr);
    stream_type_ = StreamType::STDERR;
}

FILE* StreamCapture::getStreamFile()
{
    return stream_type_ == StreamType::STDOUT ? stdout : stderr;
}

}  // namespace log
}  // namespace mw
}  // namespace bmw
