// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************



#include "platform/aas/mw/log/log_stream.h"

#include "platform/aas/mw/log/log_stream_factory.h"
#include "platform/aas/mw/log/recorder_mock.h"
#include "platform/aas/mw/log/runtime.h"
#include "platform/aas/mw/log/test/my_custom_lib/my_custom_type_mw_log.h"
#include "platform/aas/mw/log/test/sample_ara_core_logging/sample_ara_core_logging.h"

#include "amsr/generic/generic_error_domain.h"
#include "ara/core/string_view.h"

#include "gtest/gtest.h"

#include <chrono>
#include <string_view>

namespace bmw
{
namespace mw
{
namespace log
{
namespace
{

using bmw::mw::log::bmw_ext::operator<<;
using ::testing::_;
using ::testing::Return;
using namespace std::chrono_literals;
using ::testing::Types;

const SlotHandle HANDLE{42};

TEST(LogStream, CorrectlyHandleStartStop)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability to start and stop stream.");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecorderMock recorder_mock_{};
    detail::Runtime::SetRecorder(&recorder_mock_);

    EXPECT_CALL(recorder_mock_, StartRecord(amp::string_view{"Bar"}, LogLevel::kError)).WillOnce(Return(HANDLE));
    EXPECT_CALL(recorder_mock_, StopRecord(HANDLE)).Times(1);

    detail::LogStreamFactory::GetStream(LogLevel::kError, "Bar");
}

bool OtherFunctionThatLogs()
{
    detail::LogStreamFactory::GetStream(LogLevel::kError) << false;
    return true;
}

TEST(LogStream, CanLogRecursive)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies that logging recursively calls the normal recorder");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecorderMock recorder_mock_{};
    detail::Runtime::SetRecorder(&recorder_mock_);

    EXPECT_CALL(recorder_mock_, StartRecord(amp::string_view{"DFLT"}, LogLevel::kError)).WillRepeatedly(Return(HANDLE));
    EXPECT_CALL(recorder_mock_, StopRecord(HANDLE)).Times(2);

    // Expecting that we log twice via the normal recorder
    EXPECT_CALL(recorder_mock_, LogBool(HANDLE, _)).Times(2);

    // When logging a value recursively
    detail::LogStreamFactory::GetStream(LogLevel::kError) << OtherFunctionThatLogs();
}

using DurationTypes = ::testing::Types<std::chrono::nanoseconds,
                                       std::chrono::microseconds,
                                       std::chrono::milliseconds,
                                       std::chrono::seconds,
                                       std::chrono::minutes,
                                       std::chrono::hours>;

template <typename T>
class DurationTest : public testing::Test
{
  public:
    ~DurationTest() {}

  public:
    LogStream Unit() { return detail::LogStreamFactory::GetStream(LogLevel::kError); }
    DurationTest()
    {
        EXPECT_CALL(recorder_mock_, StartRecord(amp::string_view{"DFLT"}, _)).WillOnce(Return(HANDLE));
        EXPECT_CALL(recorder_mock_, StopRecord(HANDLE)).Times(1);

        detail::Runtime::SetRecorder(&recorder_mock_);
    }

    RecorderMock recorder_mock_{};
};

TYPED_TEST_SUITE_P(DurationTest);

TYPED_TEST_P(DurationTest, insertion_operator_chrono_duration)
{
    this->RecordProperty("ParentRequirement", ", ");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "ara log shall be able to log chrono duration with unit suffix");
    this->RecordProperty("TestType", "Requirements-based test");
    this->RecordProperty("DerivationTechnique", "Analysis of requirements");

    // changing representation to double to prevent amiguity between int*, float and double types
    using d_TypeParam = std::chrono::duration<double, typename TypeParam::period>;
    d_TypeParam d = std::chrono::duration_cast<d_TypeParam>(1min);
    this->Unit() << d;
}

REGISTER_TYPED_TEST_SUITE_P(DurationTest, insertion_operator_chrono_duration);

INSTANTIATE_TYPED_TEST_SUITE_P(BMW, DurationTest, DurationTypes, );

TEST(LogStream, WhenTryToGetStreamWithEmptyStringViewShallReturnDfltStream)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Getting stream with empty string view shall return the default context id.");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecorderMock recorder_mock_{};
    detail::Runtime::SetRecorder(&recorder_mock_);

    EXPECT_CALL(recorder_mock_, StartRecord(amp::string_view{"DFLT"}, LogLevel::kError)).WillOnce(Return(HANDLE));
    EXPECT_CALL(recorder_mock_, StopRecord(HANDLE)).Times(1);

    detail::LogStreamFactory::GetStream(LogLevel::kError, amp::string_view{});
}

TEST(LogStream, TypeSupport)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the support for logging various types.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto ensure_that_log_stream_reports_support_for = [](auto value) {
        using ValueType = decltype(value);
        EXPECT_TRUE(LogStreamSupports<ValueType>())
            << "LogStream is expected to support type `" << typeid(ValueType).name() << "`";
        EXPECT_TRUE(LogStreamSupports<ValueType&>())
            << "LogStream is expected to support type `" << typeid(ValueType).name() << "&`";
        EXPECT_TRUE(LogStreamSupports<ValueType&&>())
            << "LogStream is expected to support type `" << typeid(ValueType).name() << "&&`";
        EXPECT_TRUE(LogStreamSupports<const ValueType>())
            << "LogStream is expected to support type `const " << typeid(ValueType).name() << "`";
        EXPECT_TRUE(LogStreamSupports<const ValueType&>())
            << "LogStream is expected to support type `const " << typeid(ValueType).name() << "&`";
        EXPECT_TRUE(LogStreamSupports<const ValueType&&>())
            << "LogStream is expected to support type `const " << typeid(ValueType).name() << "&&`";
    };

    ensure_that_log_stream_reports_support_for(bool{});
    ensure_that_log_stream_reports_support_for(float{});
    ensure_that_log_stream_reports_support_for(double{});
    ensure_that_log_stream_reports_support_for(std::int8_t{});
    ensure_that_log_stream_reports_support_for(std::int16_t{});
    ensure_that_log_stream_reports_support_for(std::int32_t{});
    ensure_that_log_stream_reports_support_for(std::int64_t{});
    ensure_that_log_stream_reports_support_for(std::uint8_t{});
    ensure_that_log_stream_reports_support_for(std::uint16_t{});
    ensure_that_log_stream_reports_support_for(std::uint32_t{});
    ensure_that_log_stream_reports_support_for(std::uint64_t{});
    ensure_that_log_stream_reports_support_for(LogBin8{{}});
    ensure_that_log_stream_reports_support_for(LogBin16{{}});
    ensure_that_log_stream_reports_support_for(LogBin32{{}});
    ensure_that_log_stream_reports_support_for(LogBin64{{}});
    ensure_that_log_stream_reports_support_for(LogHex8{{}});
    ensure_that_log_stream_reports_support_for(LogHex16{{}});
    ensure_that_log_stream_reports_support_for(LogHex32{{}});
    ensure_that_log_stream_reports_support_for(LogHex64{{}});
    ensure_that_log_stream_reports_support_for(std::string{});
    ensure_that_log_stream_reports_support_for(amp::pmr::string{});
    ensure_that_log_stream_reports_support_for(amp::string_view{});
    ensure_that_log_stream_reports_support_for(std::string_view{});
    ensure_that_log_stream_reports_support_for(ara::core::StringView{});
    ensure_that_log_stream_reports_support_for(std::array<char, 123U>{});
    ensure_that_log_stream_reports_support_for(std::array<const char, 456U>{});
    ensure_that_log_stream_reports_support_for(mw::log::LogString{nullptr, 0U});
    ensure_that_log_stream_reports_support_for(mw::log::LogRawBuffer{nullptr, 0U});
    ensure_that_log_stream_reports_support_for(mw::log::LogSlog2Message{0U, amp::string_view{}});

    struct MyCustomType
    {
    };
    EXPECT_FALSE(LogStreamSupports<MyCustomType>());
    EXPECT_FALSE(LogStreamSupports<MyCustomType&>());
    EXPECT_FALSE(LogStreamSupports<MyCustomType&&>());
    EXPECT_FALSE(LogStreamSupports<const MyCustomType>());
    EXPECT_FALSE(LogStreamSupports<const MyCustomType&>());
    EXPECT_FALSE(LogStreamSupports<const MyCustomType&&>());
}

class LogStreamFixture : public ::testing::Test
{
  public:
    LogStream Unit() { return detail::LogStreamFactory::GetStream(LogLevel::kError); }
    LogStreamFixture()
    {
        EXPECT_CALL(recorder_mock_, StartRecord(amp::string_view{"DFLT"}, _)).WillOnce(Return(HANDLE));
        EXPECT_CALL(recorder_mock_, StopRecord(HANDLE)).Times(1);

        detail::Runtime::SetRecorder(&recorder_mock_);
    }

    RecorderMock recorder_mock_{};
};

TEST_F(LogStreamFixture, CanLogBool)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging boolean value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    auto constexpr value = true;

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogBool(HANDLE, value)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogUint8)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging unsigned int8 value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    auto constexpr value = std::uint8_t{5};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogUint8(HANDLE, value)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogInt8)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging int8 value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    auto constexpr value = std::int8_t{5};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogInt8(HANDLE, value)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogUint16)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging unsigned int16 value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    auto constexpr value = std::uint16_t{5};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogUint16(HANDLE, value)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogInt16)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging int16 value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    auto constexpr value = std::int16_t{5};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogInt16(HANDLE, value)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogUint32)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging unsigned int32 value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    auto constexpr value = std::uint32_t{5};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogUint32(HANDLE, value)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogInt32)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging int32 value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    auto constexpr value = std::int32_t{5};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogInt32(HANDLE, value)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogUint64)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging unsigned int64 value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    auto constexpr value = std::uint64_t{5};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogUint64(HANDLE, value)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogInt64)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging int64 value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    auto constexpr value = std::int64_t{5};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogInt64(HANDLE, value)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogFloat)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging float value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    auto constexpr value = float{5.2F};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogFloat(HANDLE, value)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogDouble)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging double value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    auto constexpr value = double{5.2};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogDouble(HANDLE, value)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogAraStringView)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging ara::core::StringView value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    auto value = ara::core::StringView{"Foo"};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogStringView(HANDLE, amp::string_view{"Foo"})).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogAmpStringView)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging amp::string_view value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    auto value = amp::string_view{"Foo"};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogStringView(HANDLE, amp::string_view{"Foo"})).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogStdStringView)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging std::string_view value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    auto value = amp::string_view{"Foo"};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogStringView(HANDLE, amp::string_view{"Foo"})).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, WhenTryToLogEmptyAraStringViewShallNotLog)
{
    RecordProperty("ParentRequirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies that in case of empty ara::core::StringView we shall expect no logs.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    auto value = ara::core::StringView{};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogStringView).Times(0);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, WhenTryToLogEmptyAmpStringViewShallNotLog)
{
    RecordProperty("ParentRequirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies that in case of empty amp::string_view we shall expect no logs.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    auto value = amp::string_view{};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogStringView).Times(0);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, WhenTryToLogEmptyStdStringViewShallNotLog)
{
    RecordProperty("ParentRequirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies that in case of empty std::string_view we shall expect no logs.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    auto value = std::string_view{};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogStringView).Times(0);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogConstStringReference)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging const string reference.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    const auto value = std::string{"Foo"};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogStringView(HANDLE, amp::string_view{value})).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogStdArrayOfChar)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging std::array<char> value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Expecting that the expected value will be transferred to the correct log call
    ::testing::InSequence in_sequence{};
    EXPECT_CALL(recorder_mock_, LogStringView(HANDLE, amp::string_view{"Test"})).Times(1);
    EXPECT_CALL(recorder_mock_, LogStringView(HANDLE, amp::string_view{"Twice"})).Times(1);

    // When logging the value twice
    Unit() << std::array<char, 4U>{'T', 'e', 's', 't'} << std::array<const char, 5U>{'T', 'w', 'i', 'c', 'e'};
}

TEST_F(LogStreamFixture, CanLogCharArrayLiteral)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging char[] literal value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Expecting that the expected value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogStringView(HANDLE, amp::string_view{__func__})).Times(2);

    // When logging the value once directly and once via convenience method (which avoids array-to-pointer decay)
    Unit() << __func__ << mw::log::LogStr(__func__);
}

TEST_F(LogStreamFixture, CanLogPtrToNonConstChar)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging pointer to non-const char.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    std::array<char, 4> value{'F', 'o', 'o', '\0'};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogStringView(HANDLE, amp::string_view{value.data(), 3})).Times(1);

    // When logging the value
    Unit() << value.data();
}

TEST_F(LogStreamFixture, CanLogStringLiteral)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging string literal value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    LogString::CharPtr value = "Foo";

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogStringView(HANDLE, amp::string_view{value})).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, WhenTryToLogEmptyStringLiteralShallNotLog)
{
    RecordProperty("ParentRequirement", ", ");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies that in case of empty string literal we shall expect no logs.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    // Given a value we want to log
    LogString::CharPtr value = nullptr;

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogStringView).Times(0);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, LogStreamMoveConstructorShallDetachMovedFromInstance)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of log value using a moved LogStream instance.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // When logging the value
    auto log_stream_moved_from = Unit();
    auto log_stream_move_constructed = std::move(log_stream_moved_from);

    // Given a value we want to log
    auto value = "Foo";

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogStringView(HANDLE, amp::string_view{value})).Times(1);

    log_stream_move_constructed << value;
    // The fixture shall ensure that the StopRecord is only called once.
}

TEST_F(LogStreamFixture, CanLogHex8)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging unsigned int8 in hexdecimal representation.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given a value we want to log
    const LogHex8 value{0xFF};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogUint8(HANDLE, value.value)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogHex16)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging unsigned int16 in hexdecimal representation.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given a value we want to log
    const LogHex16 value{0xFFFF};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogUint16(HANDLE, value.value)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogHex32)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging unsigned int32 in hexdecimal representation.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given a value we want to log
    const LogHex32 value{0xFFFFFF};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogUint32(HANDLE, value.value)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogHex64)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging unsigned int64 in hexdecimal representation.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given a value we want to log
    const LogHex64 value{0xFFFFFFFF};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogUint64(HANDLE, value.value)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogBin8)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging unsigned int8 in binary representation.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given a value we want to log
    const LogBin8 value{0xFF};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogUint8(HANDLE, value.value)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogBin16)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging unsigned int16 in binary representation.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given a value we want to log
    const LogBin16 value{0xFFFF};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogUint16(HANDLE, value.value)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogBin32)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging unsigned int32 in binary representation.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given a value we want to log
    const LogBin32 value{0xFFFFFF};

    EXPECT_CALL(recorder_mock_, LogUint32(HANDLE, value.value)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogBin64)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging unsigned int64 in binary representation.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given a value we want to log
    const LogBin64 value{0xFFFFFFFF};

    EXPECT_CALL(recorder_mock_, LogUint64(HANDLE, value.value)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogRawBuffer)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging LogRawBuffer.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given a value we want to log
    char s2[]{"1234"};
    const LogRawBuffer value{s2, sizeof(s2)};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, Log_LogRawBuffer(HANDLE, value.data(), static_cast<uint64_t>(value.size()))).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, WhenTryToLogEmptyRawBufferShallNotLog)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Logging empty LogRawBuffer shall not log.");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given a value we want to log
    const LogRawBuffer value{nullptr, 1};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, Log_LogRawBuffer(HANDLE, value.data(), static_cast<uint64_t>(value.size()))).Times(0);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogACustomType)
{
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of logging custom type (struct).");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    ::testing::InSequence in_sequence{};

    const auto operator_string0 = "my_custom_type: int_field : ";
    const auto operator_string1 = " , string_field : ";
    // Given a custom object we want to log
    const my::custom::type::MyCustomType value{12, "hello, world"};

    // Expecting that the custom output operator overload will be called which will be transfer the contained values to
    // the correct log calls
    EXPECT_CALL(recorder_mock_, LogStringView(HANDLE, amp::string_view{operator_string0})).Times(1);
    EXPECT_CALL(recorder_mock_, LogInt32(HANDLE, value.int_field)).Times(1);
    EXPECT_CALL(recorder_mock_, LogStringView(HANDLE, amp::string_view{operator_string1})).Times(1);
    EXPECT_CALL(recorder_mock_,
                LogStringView(HANDLE, amp::string_view{value.string_field.c_str(), value.string_field.size()}))
        .Times(1);

    // When logging the value
    Unit() << value;
}

TEST(LogStreamFlush, WhenFlushingLogStreamAfterLogUint8ShallBeAbleToLogBoolAgain)
{
    testing::InSequence seq;
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies teh ability of flushing LogStream used to log unsigned int8 and then use it again to log "
                   "boolean value.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given a value we want to log
    constexpr auto value_uint8 = std::uint8_t{5};
    constexpr auto value_bool = true;

    RecorderMock recorder_mock_{};
    detail::Runtime::SetRecorder(&recorder_mock_);

    EXPECT_CALL(recorder_mock_, StartRecord(amp::string_view{"DFLT"}, _)).WillOnce(Return(HANDLE));

    auto log_stream{detail::LogStreamFactory::GetStream(LogLevel::kError, amp::string_view("DFLT"))};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogUint8(HANDLE, value_uint8)).Times(1);
    log_stream << value_uint8;

    EXPECT_CALL(recorder_mock_, StopRecord(HANDLE)).Times(1);
    EXPECT_CALL(recorder_mock_, StartRecord(amp::string_view{"DFLT"}, _)).WillOnce(Return(HANDLE));

    log_stream.Flush();

    EXPECT_CALL(recorder_mock_, LogBool(HANDLE, value_bool)).Times(1);
    log_stream << value_bool;

    EXPECT_CALL(recorder_mock_, StopRecord(HANDLE)).Times(1);
}

TEST(LogStreamFlush, AvoidFormattingCallsWhenSlotIsNotAvailable)
{
    testing::InSequence seq;
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the inability of logging formatting functions if no slots are reserved.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given a value we want to log
    constexpr auto value_uint8 = std::uint8_t{5};
    constexpr auto value_bool = true;

    RecorderMock recorder_mock_{};
    detail::Runtime::SetRecorder(&recorder_mock_);

    EXPECT_CALL(recorder_mock_, StartRecord(amp::string_view{"DFLT"}, _)).WillOnce(Return(amp::nullopt));

    auto log_stream{detail::LogStreamFactory::GetStream(LogLevel::kError, amp::string_view("DFLT"))};

    //  Without slot available formatting function shall not be called:
    EXPECT_CALL(recorder_mock_, LogUint8(HANDLE, value_uint8)).Times(0);
    log_stream << value_uint8;

    EXPECT_CALL(recorder_mock_, StopRecord(HANDLE)).Times(0);
    EXPECT_CALL(recorder_mock_, StartRecord(amp::string_view{"DFLT"}, _)).WillOnce(Return(amp::nullopt));

    log_stream.Flush();

    //  Without slot available formatting function shall not be called:
    EXPECT_CALL(recorder_mock_, LogBool(HANDLE, value_bool)).Times(0);
    log_stream << value_bool;

    //  Nor is StopRecord function expected to be called
    EXPECT_CALL(recorder_mock_, StopRecord(HANDLE)).Times(0);
}

TEST(LogStreamFlush, WhenEmptyAppIdStringProvidedExpectDefaultOneReturned)
{
    testing::InSequence seq;
    RecordProperty("ParentRequirement", "");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "When empty app id is provided the default context id shall be returned.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given a value we want to log
    constexpr auto value_uint8 = std::uint8_t{5};

    RecorderMock recorder_mock_{};
    detail::Runtime::SetRecorder(&recorder_mock_);

    EXPECT_CALL(recorder_mock_, StartRecord(amp::string_view{"DFLT"}, _)).WillOnce(Return(HANDLE));

    //  Here we provide empty AppId string:
    auto log_stream{detail::LogStreamFactory::GetStream(LogLevel::kError, amp::string_view(nullptr, 0UL))};

    //  Standard formatting function shall be called:
    EXPECT_CALL(recorder_mock_, LogUint8(HANDLE, value_uint8));
    log_stream << value_uint8;

    EXPECT_CALL(recorder_mock_, StopRecord(HANDLE));
}

enum class UColor : std::uint64_t
{
    red = 100U,
    green = 200U,
    blue = 300U
};
enum class IColor : std::uint16_t
{
    red = 400,
    green = 500,
    blue = 600
};

TEST_F(LogStreamFixture, CanLogAnEnumClassWithUnderlyingType)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging an enum class with underlying type (uint).");
    RecordProperty("TestingTechnique", "Internal-implementation-based test");

    UColor value{UColor::green};

    EXPECT_CALL(recorder_mock_, LogUint64(HANDLE, _)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogAnEnumClassWithoutUnderlyingType)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verify the ability of logging an enum class without underlying type (directly using int).");
    RecordProperty("TestingTechnique", "Internal-implementation-based test");

    IColor value{IColor::green};

    EXPECT_CALL(recorder_mock_, LogInt32(HANDLE, _)).Times(1);

    // When logging the value
    Unit() << value;
}

TEST_F(LogStreamFixture, CanLogAraCoreResultWithValue)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging ara::core::Result with int32 value.");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    ::testing::InSequence in_sequence{};

    const auto operator_string = "Result value: ";

    // Given a result with value we want to log
    const ara::core::Result<std::int32_t> result{123};

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogStringView(HANDLE, amp::string_view{operator_string})).Times(1);
    EXPECT_CALL(recorder_mock_, LogInt32(HANDLE, result.Value())).Times(1);

    // When logging the value
    Unit() << result;
}

TEST_F(LogStreamFixture, CanLogAraCoreResultWithError)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging ara::core::Result with error value.");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    ::testing::InSequence in_sequence{};

    const auto operator_string = "Error message: ";

    // Given a result with value we want to log
    const ara::core::Result<std::int32_t> result =
        ara::core::Result<int>::FromError(amsr::generic::GenErrc::kSystemPrivileges);

    // Expecting that this value will be transferred to the correct log call
    EXPECT_CALL(recorder_mock_, LogStringView(HANDLE, amp::string_view{operator_string})).Times(1);

    const auto& msg = result.Error().UserMessage();
    amp::string_view expected_msg;
    if (msg.size())
    {
        expected_msg = {msg.data(), msg.size()};
    }
    else
    {
        expected_msg = "{EMPTY}";
    }
    EXPECT_CALL(recorder_mock_, LogStringView(HANDLE, expected_msg)).Times(1);

    // When logging the value
    Unit() << result;
}

TEST_F(LogStreamFixture, UsesFallbackRecorderWithinOtherRecorder)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies that the fallback recorder is used, if another recorder also uses logging");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Expecting that we log once via the normal recorder
    EXPECT_CALL(recorder_mock_, LogBool(HANDLE, true)).WillOnce([this](auto, auto logged_value) {
        EXPECT_TRUE(logged_value);

        // When logging within a recorder
        Unit() << false;
    });

    // When logging a value
    Unit() << true;

    // Then the recorder_mock is only invoked once, not multiple times.
}

}  // namespace
}  // namespace log
}  // namespace mw
}  // namespace bmw
