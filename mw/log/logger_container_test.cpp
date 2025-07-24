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



#include "platform/aas/mw/log/logger_container.h"
#include "platform/aas/mw/log/logger.h"

#include "platform/aas/mw/log/logging.h"
#include "platform/aas/mw/log/recorder_mock.h"

#include <amp_string_view.hpp>

#include "gtest/gtest.h"
#include <thread>

namespace bmw
{
namespace mw
{
namespace log
{

namespace
{
const auto kContext1 = amp::string_view{"MYCT"};
const auto kDefaultContext = amp::string_view{"DFLT"};
}  // namespace

TEST(LoggerContainerTests, WhenRequestingNonExistingNewLoggerItShallBeInsertedAndReturnToCaller)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verifies the ability of requesting non-existing new logger shall be inserted and returned to the caller.");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    LoggerContainer unit;
    EXPECT_EQ(unit.GetLogger(kContext1).GetContext(), kContext1);
}

TEST(LoggerContainerTests, WhenGetingDefaultLoggerShallGetDLFTContextID)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies that when getting a default logger it shall get DFLT context id.");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    LoggerContainer unit;
    EXPECT_EQ(unit.GetDefaultLogger().GetContext(), kDefaultContext);
}

TEST(LoggerContainerTests, WhenRequestingAlreadyExistingLoggerShallBeReturnedWithoutInsertingNewLogger)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verifies that when requesting already existing logger shall be returned without inserting new logger.");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    LoggerContainer unit;
    EXPECT_EQ(unit.GetLogger(kContext1).GetContext(), kContext1);
    EXPECT_EQ(unit.GetLogger(kContext1).GetContext(), kContext1);
}

TEST(LoggerContainerTests, WhenLoggerContainerIsFullShallGetDefaultContextWhenNewLoggerRequested)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verifies that when logger container is full shall get default context when new logger is requested.");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Expecting a log record of level verbose
    LoggerContainer unit;
    std::vector<std::string> contexts_vector(unit.GetCapacity());

    for (size_t i = 0; i < contexts_vector.size(); i++)
    {
        contexts_vector[i] = std::to_string(i);
    }
    // iterate to make the buffer full with 31 different context.
    for (const auto& context : contexts_vector)
    {
        // filling the buffer with different contexts till overrun.
        EXPECT_EQ(unit.GetLogger(context).GetContext(), amp::string_view{context});
    }
    // buffer is full, default logger is returned.
    EXPECT_EQ(unit.GetLogger(kContext1).GetContext(), kDefaultContext);
    amp::string_view inserted_context = contexts_vector[0];
    EXPECT_EQ(unit.GetLogger(inserted_context).GetContext(), inserted_context);
}

void LoggerRequester1(LoggerContainer& logger_container)
{
    EXPECT_EQ(logger_container.GetLogger(kContext1).GetContext(), kContext1);
}

void LoggerRequester2(LoggerContainer& logger_container)
{
    EXPECT_EQ(logger_container.GetLogger(kContext1).GetContext(), kContext1);
}

TEST(LoggerContainerTests, WhenTwoThreadsRequestSameLoggerShallBeOnlyOneExistingInLoggerContainer)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verifies that when two threads request the same logger it shall be only one existing in logger container.");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    LoggerContainer unit;

    std::thread t1(LoggerRequester1, std::ref(unit));
    t1.join();

    std::thread t2(LoggerRequester2, std::ref(unit));
    t2.join();

    // default logger exists
    EXPECT_EQ(unit.GetLogger(kDefaultContext).GetContext(), kDefaultContext);
}

}  // namespace log
}  // namespace mw
}  // namespace bmw
