#include <array>
#include <cstdint>
#include <memory>
#include <tuple>

#include <opentelemetry/logs/event_id.h>
#include <opentelemetry/logs/provider.h>
#include <opentelemetry/logs/severity.h>
#include <opentelemetry/nostd/string_view.h>
#include <opentelemetry/sdk/logs/logger_provider.h>
#include <opentelemetry/sdk/logs/simple_log_record_processor.h>
#include <opentelemetry/trace/span_id.h>
#include <opentelemetry/trace/trace_id.h>

#include <syslog.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "opentelemetry/exporters/wwa/syslog/log_record_exporter_factory.h"
#include "opentelemetry/exporters/wwa/syslog/syslog_interface.h"

namespace {

class MockedSyslog : public wwa::opentelemetry::exporter::logs::SyslogInterface {
public:
    MOCK_METHOD(void, openlog, (opentelemetry::nostd::string_view ident, int option, int facility), (override));
    MOCK_METHOD(void, syslog, (int priority, opentelemetry::nostd::string_view message), (override));
    MOCK_METHOD(void, closelog, (), (override));
};

class SyslogExporterTest : public testing::Test {
protected:
    void SetUp() override
    {
        const opentelemetry::nostd::string_view ident = "name";
        const int option                              = LOG_CONS | LOG_PID;
        const int facility                            = LOG_LOCAL0;

        EXPECT_CALL(*this->m_syslog, openlog(testing::Eq(ident), testing::Eq(option), testing::Eq(facility)))
            .Times(testing::Exactly(1));

        auto exporter = wwa::opentelemetry::exporter::logs::SyslogLogRecordExporterFactory::Create(
            ident, std::static_pointer_cast<wwa::opentelemetry::exporter::logs::SyslogInterface>(this->m_syslog),
            option, facility
        );

        auto processor = std::make_unique<opentelemetry::sdk::logs::SimpleLogRecordProcessor>(std::move(exporter));
        auto provider  = std::make_shared<opentelemetry::sdk::logs::LoggerProvider>(std::move(processor));
        opentelemetry::logs::Provider::SetLoggerProvider(
            std::static_pointer_cast<opentelemetry::logs::LoggerProvider>(provider)
        );
    }

    void TearDown() override
    {
        EXPECT_CALL(*this->m_syslog, closelog()).Times(testing::Exactly(1));

        const std::shared_ptr<opentelemetry::logs::LoggerProvider> provider;
        opentelemetry::logs::Provider::SetLoggerProvider(provider);

        testing::Mock::VerifyAndClearExpectations(this->m_syslog.get());

        wwa::opentelemetry::exporter::logs::SyslogLogRecordExporterFactory::setSyslogImplementation(nullptr);
    }

    // NOLINTNEXTLINE(*-non-private-member-variables-in-classes)
    std::shared_ptr<MockedSyslog> m_syslog{std::make_shared<MockedSyslog>()};
};

class ParametrizedSyslogExporterTest
    : public SyslogExporterTest,
      public testing::WithParamInterface<std::tuple<opentelemetry::logs::Severity, int>> {};

TEST_F(SyslogExporterTest, BasicTest)
{
    const opentelemetry::nostd::string_view name = "somelib";
    const opentelemetry::nostd::string_view msg  = "test message";
    const std::string expected                   = "[" + std::string(name) + "] " + std::string(msg);

    EXPECT_CALL(*this->m_syslog, syslog(testing::Eq(LOG_INFO), testing::Eq(expected))).Times(testing::Exactly(1));

    auto logger = opentelemetry::logs::Provider::GetLoggerProvider()->GetLogger("logger name", name);
    logger->Info("test message");
}

TEST_F(SyslogExporterTest, AdvancedTest)
{
    // NOLINTBEGIN(*-magic-numbers)
    const opentelemetry::nostd::string_view name    = "somelib";
    const opentelemetry::nostd::string_view version = "1.0";
    const std::vector<opentelemetry::nostd::string_view> message{"Part 1", "Part 2"};
    std::int64_t event_id                              = 12345;
    const opentelemetry::nostd::string_view event_name = "event_name";

    std::array<const std::uint8_t, opentelemetry::trace::TraceId::kSize> trace_id_raw = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
    };

    std::array<const std::uint8_t, opentelemetry::trace::SpanId::kSize> span_id_raw = {0x11, 0x12, 0x13, 0x14,
                                                                                       0x15, 0x16, 0x17, 0x18};
    // NOLINTEND(*-magic-numbers)

    const std::string expected = "[" + std::string(name) + "/" + std::string(version) + "] " + std::string(message[0]) +
                                 ", " + std::string(message[1]) + " [EV: " + std::string(event_name) + " (" +
                                 std::to_string(event_id) +
                                 "), TID: 0102030405060708090a0b0c0d0e0f10, SID: 1112131415161718]";

    EXPECT_CALL(*this->m_syslog, syslog(testing::Eq(LOG_INFO), testing::Eq(expected))).Times(testing::Exactly(1));

    auto logger = opentelemetry::logs::Provider::GetLoggerProvider()->GetLogger("logger name", name, version);
    logger->Info(
        message, opentelemetry::trace::TraceId(trace_id_raw), opentelemetry::trace::SpanId(span_id_raw),
        opentelemetry::logs::EventId(event_id, event_name)
    );
}

TEST_P(ParametrizedSyslogExporterTest, MappingTest)
{
    opentelemetry::logs::Severity severity;  // NOLINT(*-init-variables)
    int priority;                            // NOLINT(*-init-variables)
    std::tie(severity, priority) = GetParam();

    const opentelemetry::nostd::string_view name = "somelib";
    const opentelemetry::nostd::string_view msg  = "test message";

    EXPECT_CALL(*this->m_syslog, syslog(testing::Eq(priority), testing::HasSubstr(msg))).Times(testing::Exactly(1));

    auto logger = opentelemetry::logs::Provider::GetLoggerProvider()->GetLogger("logger name", name);
    logger->Log(severity, "test message");
}

INSTANTIATE_TEST_SUITE_P(
    Table, ParametrizedSyslogExporterTest,
    testing::Values(
        std::make_tuple(opentelemetry::logs::Severity::kInvalid, LOG_ERR),

        std::make_tuple(opentelemetry::logs::Severity::kTrace, LOG_DEBUG),
        std::make_tuple(opentelemetry::logs::Severity::kTrace2, LOG_DEBUG),
        std::make_tuple(opentelemetry::logs::Severity::kTrace3, LOG_DEBUG),
        std::make_tuple(opentelemetry::logs::Severity::kTrace4, LOG_DEBUG),

        std::make_tuple(opentelemetry::logs::Severity::kDebug, LOG_DEBUG),
        std::make_tuple(opentelemetry::logs::Severity::kDebug2, LOG_DEBUG),
        std::make_tuple(opentelemetry::logs::Severity::kDebug3, LOG_DEBUG),
        std::make_tuple(opentelemetry::logs::Severity::kDebug4, LOG_DEBUG),

        std::make_tuple(opentelemetry::logs::Severity::kInfo, LOG_INFO),
        std::make_tuple(opentelemetry::logs::Severity::kInfo2, LOG_INFO),
        std::make_tuple(opentelemetry::logs::Severity::kInfo3, LOG_NOTICE),
        std::make_tuple(opentelemetry::logs::Severity::kInfo4, LOG_NOTICE),

        std::make_tuple(opentelemetry::logs::Severity::kWarn, LOG_WARNING),
        std::make_tuple(opentelemetry::logs::Severity::kWarn2, LOG_WARNING),
        std::make_tuple(opentelemetry::logs::Severity::kWarn3, LOG_WARNING),
        std::make_tuple(opentelemetry::logs::Severity::kWarn4, LOG_WARNING),

        std::make_tuple(opentelemetry::logs::Severity::kError, LOG_ERR),
        std::make_tuple(opentelemetry::logs::Severity::kError2, LOG_ERR),
        std::make_tuple(opentelemetry::logs::Severity::kError3, LOG_CRIT),
        std::make_tuple(opentelemetry::logs::Severity::kError4, LOG_ALERT),

        std::make_tuple(opentelemetry::logs::Severity::kFatal, LOG_EMERG),
        std::make_tuple(opentelemetry::logs::Severity::kFatal2, LOG_EMERG),
        std::make_tuple(opentelemetry::logs::Severity::kFatal3, LOG_EMERG),
        std::make_tuple(opentelemetry::logs::Severity::kFatal4, LOG_EMERG)
    )
);

}  // namespace
