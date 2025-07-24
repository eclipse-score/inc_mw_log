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


#include "platform/aas/mw/log/detail/logging_identifier.h"

#include "gtest/gtest.h"

#include <cstring>

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

TEST(LoggingIdentifierTestSuite, CheckThatLongIdentifiersShallBeCropped)
{
    RecordProperty("ParentRequirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "logging identifier has maximum length in which the identifier will be cropped.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    LoggingIdentifier identifier{"12345"};
    EXPECT_EQ(identifier.GetStringView(), amp::string_view{"1234"});
}

TEST(LoggingIdentifierTestSuite, CheckThatHashMatchesIntHasher)
{
    RecordProperty("ParentRequirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies that hash matches int hasher.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    amp::string_view ctx{"CTX1"};
    std::int32_t val{};
    std::memcpy(&val, ctx.data(), sizeof(val));
    LoggingIdentifier identifier{ctx};
    EXPECT_EQ(LoggingIdentifier::HashFunction{}(identifier), std::hash<std::int32_t>{}(val));
}

TEST(LoggingIdentifierTestSuite, EqualityOperatorShallReturnTrueForTheSameString)
{
    RecordProperty("ParentRequirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "logging identifiers with the same context name shall be equal.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    amp::string_view ctx{"CTX1"};
    LoggingIdentifier lhs{ctx};
    LoggingIdentifier rhs{ctx};
    EXPECT_EQ(lhs, rhs);
}

TEST(LoggingIdentifierTestSuite, InequalityOperatorShallReturnTrueForDifferentStrings)
{
    RecordProperty("ParentRequirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "logging identifiers with the different context name shall not be equal.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    LoggingIdentifier lhs{amp::string_view{"CTX1"}};
    LoggingIdentifier rhs{amp::string_view{"CTX"}};
    EXPECT_TRUE(lhs != rhs);
}

TEST(LoggingIdentifierTestSuite, AssignOperatorShallCopyLoggingIdentifier)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check the assign operator functionality.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    amp::string_view ctx{"CTX1"};
    amp::string_view ctx2{"CTX2"};
    LoggingIdentifier identifier{ctx};
    identifier = LoggingIdentifier{ctx2};
    EXPECT_EQ(identifier.GetStringView(), ctx2);
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
