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


#include "platform/aas/mw/log/detail/log_record.h"

#include "amp_optional.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{
namespace
{
constexpr std::size_t kMaxPayloadSize = 64U;
constexpr std::size_t kMaxPayloadSizeBiggerSrc = 256U;
constexpr std::size_t kMaxPayloadSizeSmallerSrc = 16U;

class LogRecordCopyAndMoveOperatorsFixture : public ::testing::TestWithParam<std::size_t>
{
  protected:
    LogRecord GetSource() const noexcept
    {
        LogRecord src{GetParam()};
        src.getLogEntry().payload.resize(GetParam() - GetSourceCapacity());
        return src;
    }

    std::size_t GetSourceCapacity() const noexcept { return GetParam() / 2; }
};

TEST(LogRecord, LogRecordShallReturnExpectedLogEntry)
{
    RecordProperty("Requirement", ", 1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "LogRecord can be constructed with max payload size.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    LogRecord unit{kMaxPayloadSize};
    EXPECT_EQ(unit.getLogEntry().payload.capacity(), kMaxPayloadSize);
}

TEST(LogRecord, LogRecordShallReturnExpectedVerbosePayload)
{
    RecordProperty("Requirement", ", 1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "LogRecord can provide the expected verbose payload.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    LogRecord unit{kMaxPayloadSize};
    EXPECT_EQ(unit.getVerbosePayload().RemainingCapacity(), kMaxPayloadSize);
}

TEST_P(LogRecordCopyAndMoveOperatorsFixture, LogRecordShallCopyAssignAndUpdateReferenceCorrectly)
{
    RecordProperty("Requirement", ", 1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "LogRecord can provide copy assignment operator with valid state.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    LogRecord unit{kMaxPayloadSize};
    {
        // Test lifetime correctness by letting src go out of scope after assignment.
        const LogRecord src{GetSource()};
        unit = src;
    }
    EXPECT_EQ(unit.getVerbosePayload().RemainingCapacity(), GetSourceCapacity());
}

TEST_P(LogRecordCopyAndMoveOperatorsFixture, LogRecordShallCopyConstructAndUpdateReferenceCorrectly)
{
    RecordProperty("Requirement", ", 1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "LogRecord can provide copy constructor with valid state.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    amp::optional<LogRecord> unit{};
    {
        // Test lifetime correctness by letting src go out of scope after assignment.
        const LogRecord src{GetSource()};
        unit.emplace(src);
    }
    EXPECT_EQ(unit->getVerbosePayload().RemainingCapacity(), GetSourceCapacity());
}

TEST_P(LogRecordCopyAndMoveOperatorsFixture, LogRecordShallMoveAssignAndUpdateReferenceCorrectly)
{
    RecordProperty("Requirement", ", 1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "LogRecord can provide move assigment operator with validstate.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    LogRecord unit{kMaxPayloadSize};
    {
        // Test lifetime correctness by letting src go out of scope after construction.
        LogRecord src{GetSource()};
        unit = std::move(src);
    }
    EXPECT_EQ(unit.getVerbosePayload().RemainingCapacity(), GetSourceCapacity());
}

TEST_P(LogRecordCopyAndMoveOperatorsFixture, LogRecordShallMoveConstructAndUpdateReferenceCorrectly)
{
    RecordProperty("Requirement", ", 1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "LogRecord can provide move constructor with valid state.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    amp::optional<LogRecord> unit{};
    {
        // Test lifetime correctness by letting src go out of scope after construction.
        LogRecord src{GetSource()};
        unit.emplace(std::move(src));
    }
    EXPECT_EQ(unit->getVerbosePayload().RemainingCapacity(), GetSourceCapacity());
}

INSTANTIATE_TEST_SUITE_P(LogRecordMoveAndCopyTests,
                         LogRecordCopyAndMoveOperatorsFixture,
                         ::testing::Values(kMaxPayloadSizeBiggerSrc, kMaxPayloadSizeSmallerSrc));

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
