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


#ifndef PLATFORM_AAS_MW_STREAM_CAPTURE_H
#define PLATFORM_AAS_MW_STREAM_CAPTURE_H

#include <stdio.h>
#include <cstdio>
#include <string>

namespace bmw
{
namespace mw
{
namespace log
{

class StreamCapture
{
  public:
    void StartCapturingStdout();
    void StartCapturingStderr();
    std::string FetchCapturedStream();

  private:
    void StartCapturingStream(FILE* stream);
    FILE* getStreamFile();

    int old_stream_;
    bool capturing_ = false;
    std::string file_name_;
    enum class StreamType : std::uint8_t
    {
        STDOUT = 0,
        STDERR
    };
    StreamType stream_type_;
};

}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif  // PLATFORM_AAS_MW_LOG_LOGGER_H
