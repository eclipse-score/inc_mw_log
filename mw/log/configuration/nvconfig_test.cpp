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


#include "nvconfig.h"

#include "platform/aas/mw/log/log_level.h"

#include "gtest/gtest.h"

namespace
{

using NvConfig = bmw::mw::log::NvConfig;
using ReadResult = NvConfig::ReadResult;

const std::string JSON_PATH = "platform/aas/mw/log/configuration/test/data/test-class-id.json";
const std::string JSON_PATH_2 = "platform/aas/mw/log/configuration/test/data/second-test-class-id.json";
const std::string EMPTY_FILE = "platform/aas/mw/log/configuration/test/data/empty-class-id.json";
const std::string EMPTY_JSON_OBJECT = "platform/aas/mw/log/configuration/test/data/error-parse-empty-json-object.json";
const std::string WRONG_JSON_PATH = "platform/aas/mw/log/configuration/test/data/wrong-path-class-id.json";
const std::string EMPTY_JSON = "platform/aas/mw/log/configuration/test/data/empty-json-class-id.json";
const std::string ERROR_PARSE_1_PATH = "platform/aas/mw/log/configuration/test/data/error-parse-1-json-class-id.json";
const std::string ERROR_CONTENT_1_PATH =
    "platform/aas/mw/log/configuration/test/data/error-content-1-json-class-id.json";
const std::string ERROR_CONTENT_2_PATH =
    "platform/aas/mw/log/configuration/test/data/error-content-2-json-class-id.json";
const std::string ERROR_CONTENT_3_PATH =
    "platform/aas/mw/log/configuration/test/data/error-content-3-json-class-id.json";
const std::string ERROR_CONTENT_WRONG_ID_VALUE =
    "platform/aas/mw/log/configuration/test/data/error-content-wrong-id-value.json";

TEST(NonVerboseConfig, NvConfigReturnsExpectedValues)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Logging libraries use static configuration based on .json files.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc1(JSON_PATH);
    EXPECT_EQ(ReadResult::kOK, nvc1.parseFromJson());                                                // ok json results
    EXPECT_EQ(301, nvc1.getDltMsgDesc("bmw::logging::PersistentLogFileEvent")->id_msg_descriptor_);  // id value reading
    EXPECT_EQ(bmw::mw::log::LogLevel::kFatal,
              nvc1.getDltMsgDesc("bmw::logging::PersistentLogFileEvent")->logLevel_);  // loglevel value reading
    EXPECT_EQ(bmw::mw::log::LogLevel::kError,
              nvc1.getDltMsgDesc("LogMark::stdframe")->logLevel_);  // loglevel value reading other example
    EXPECT_EQ(bmw::mw::log::LogLevel::kInfo,
              nvc1.getDltMsgDesc("poseng::logging::ReprocessingCycle")
                  ->logLevel_);  // loglevel value reading when using default loglevel
    EXPECT_EQ(10, nvc1.getDltMsgDesc("LogMark::stdframe")->id_msg_descriptor_);  // id value reading other example
    EXPECT_EQ(amp::string_view{"Repr"},
              nvc1.getDltMsgDesc("aas::logging::ReprocessingCycle")->ctxid_.GetStringView());  // ctxid value reading
    EXPECT_EQ(amp::string_view{"PE"},
              nvc1.getDltMsgDesc("poseng::logging::ReprocessingCycle")->appid_.GetStringView());  // appid value reading
    EXPECT_EQ(amp::string_view{"PERL"},
              nvc1.getDltMsgDesc("bmw::logging::PersistentLogFileEvent")
                  ->ctxid_.GetStringView());  // ctxid value reading other example
    EXPECT_EQ(amp::string_view{"STDF"},
              nvc1.getDltMsgDesc("LogMark::stdframe")->appid_.GetStringView());  // appid value reading other example
}

TEST(NonVerboseConfig, NvConfigReturnsExpectedValuesWithOtherFile)
{
    RecordProperty("Requirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Logging libraries use static configuration based on .json files.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc2(JSON_PATH_2);
    EXPECT_EQ(ReadResult::kOK, nvc2.parseFromJson());                                             // ok json results
    EXPECT_EQ(8650816, nvc2.getDltMsgDesc("adp::planning::awa::DebugData")->id_msg_descriptor_);  // id value reading
    EXPECT_EQ(bmw::mw::log::LogLevel::kWarn,
              nvc2.getDltMsgDesc("adp::logging::DynamicInsight")->logLevel_);  // loglevel value reading
    EXPECT_EQ(bmw::mw::log::LogLevel::kError,
              nvc2.getDltMsgDesc("bmw::logging::timesync::DltTimeSyncTimestamp")
                  ->logLevel_);  // loglevel value reading other example
    EXPECT_EQ(8650814,
              nvc2.getDltMsgDesc("adp::perception::CrocStateTraceable")
                  ->id_msg_descriptor_);  // id value reading other example
    EXPECT_EQ(amp::string_view{"Repr"},
              nvc2.getDltMsgDesc("aas::logging::ReprocessingEvent")->ctxid_.GetStringView());  // ctxid value reading
    EXPECT_EQ(amp::string_view{"Fasi"},
              nvc2.getDltMsgDesc("bmw::sli::TsfBaseConfig")->appid_.GetStringView());  // appid value reading
    EXPECT_EQ(amp::string_view{"DTNV"},
              nvc2.getDltMsgDesc("adp::planning::driving_tube::DiagnosticLogsData")
                  ->ctxid_.GetStringView());  // ctxid value reading other example
    EXPECT_EQ(amp::string_view{"Plan"},
              nvc2.getDltMsgDesc("adp::planning::driving_tube::DiagnosticLogsData")
                  ->appid_.GetStringView());  // appid value reading other example
}

TEST(NonVerboseConfig, NvConfigReturnsErrorOpenWhenGivenEmptyFile)
{
    RecordProperty("Requirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the inability of parsing a general empty file.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc3(EMPTY_FILE);
    EXPECT_EQ(ReadResult::kERROR_PARSE, nvc3.parseFromJson());  // error parse because it is a general empty file
}

TEST(NonVerboseConfig, NvConfigReturnsErrorOpenWhenGivenPathToNonExistentFile)
{
    RecordProperty("Requirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "ara::log shall discard configuration files that are not found.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc4(WRONG_JSON_PATH);
    EXPECT_EQ(ReadResult::kERROR_PARSE, nvc4.parseFromJson());  // error parse because the file doesn't exist
}

TEST(NonVerboseConfig, NvConfigReturnsOkWhenGivenEmptyJsonFile)
{
    RecordProperty("Requirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of parsing an empty JSON file.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc5(EMPTY_JSON);
    EXPECT_EQ(ReadResult::kOK, nvc5.parseFromJson());  // ok because this json file doesn't have items
}

TEST(NonVerboseConfig, NvConfigReturnsErrorParseIfEmptyObject)
{
    RecordProperty("Requirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies the inability of parsing JSON file that has array instead of JSON object as a root node.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc6(EMPTY_JSON_OBJECT);
    EXPECT_EQ(ReadResult::kERROR_PARSE,
              nvc6.parseFromJson());  // array instead of json object as one of the values
}

TEST(NonVerboseConfig, NvConfigReturnsErrorParseIfThereIsSomethingElseInstedOfObjectAsValue)
{
    RecordProperty("Requirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the inability of parsing a JSON file that does not have object as value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc6(ERROR_PARSE_1_PATH);
    EXPECT_EQ(ReadResult::kERROR_PARSE,
              nvc6.parseFromJson());  // aray instead of json object as one of the values
}

TEST(NonVerboseConfig, NvConfigReturnsErrorContentIfCtxidValuePairDoesntExistsForOneObject)
{
    RecordProperty("Requirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verifies the inability of parsing JSON file that misses ctxid key-value pair for one of the objects.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc7(ERROR_CONTENT_1_PATH);
    EXPECT_EQ(ReadResult::kERROR_CONTENT,
              nvc7.parseFromJson());  // ctxid key-value pair is missing in one of the objects
}

TEST(NonVerboseConfig, NvConfigReturnsErrorContentIfAppidValuePairDoesntExistsForOneObject)
{
    RecordProperty("Requirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verifies the inability of parsing JSON file that misses appid key-value pair for one of the objects.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc8(ERROR_CONTENT_2_PATH);
    EXPECT_EQ(ReadResult::kERROR_CONTENT,
              nvc8.parseFromJson());  // appid key-value pair is missing in one of the objects
}

TEST(NonVerboseConfig, NvConfigReturnsErrorContentIfIdValuePairDoesntExistsForOneObject)
{
    RecordProperty("Requirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies the inability of parsing JSON file that misses id key-value pair for one of the objects.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc9(ERROR_CONTENT_3_PATH);
    EXPECT_EQ(ReadResult::kERROR_CONTENT,
              nvc9.parseFromJson());  // id key-value pair is missing in one of the objects
}

TEST(NonVerboseConfig, NvConfigReturnsErrorIfIdDataTypeIsWrong)
{
    RecordProperty("Requirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the inability of parsing JSON file that has wrong id value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    NvConfig nvc10(ERROR_CONTENT_WRONG_ID_VALUE);
    EXPECT_EQ(ReadResult::kERROR_CONTENT, nvc10.parseFromJson());  // wrong ID data type (string instead of int).
}

}  // namespace
