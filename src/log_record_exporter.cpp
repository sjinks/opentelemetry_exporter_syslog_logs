#include "log_record_exporter.h"

#include <array>
#include <string>

#include <opentelemetry/common/attribute_value.h>
#include <opentelemetry/sdk/instrumentationscope/instrumentation_scope.h>
#include <opentelemetry/sdk/logs/read_write_log_record.h>

#include "default_syslog_implementation.h"

namespace {

const std::array<int, std::extent_v<decltype(opentelemetry::logs::SeverityNumToText)>> priority_map{
    LOG_ERR,                                             // INVALID
    LOG_DEBUG,   LOG_DEBUG,   LOG_DEBUG,   LOG_DEBUG,    // TRACE, TRACE2, TRACE3, TRACE4
    LOG_DEBUG,   LOG_DEBUG,   LOG_DEBUG,   LOG_DEBUG,    // DEBUG, DEBUG2, DEBUG3, DEBUG4
    LOG_INFO,    LOG_INFO,    LOG_NOTICE,  LOG_NOTICE,   // INFO, INFO2, INFO3, INFO4
    LOG_WARNING, LOG_WARNING, LOG_WARNING, LOG_WARNING,  // WARN, WARN2, WARN3, WARN4
    LOG_ERR,     LOG_ERR,     LOG_CRIT,    LOG_ALERT,    // ERROR, ERROR2, ERROR3, ERROR4
    LOG_EMERG,   LOG_EMERG,   LOG_EMERG,   LOG_EMERG     // FATAL, FATAL2, FATAL3, FATAL4
};

std::string stringify_attribute(const opentelemetry::common::AttributeValue& v)
{
    using namespace opentelemetry::common;

    return opentelemetry::nostd::visit(
        [](auto&& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, bool>) {
                return arg ? "true" : "false";
            }

            if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t> || std::is_same_v<T, uint32_t> ||
                          std::is_same_v<T, uint64_t> || std::is_same_v<T, double>)
            {
                return std::to_string(arg);
            }

            if constexpr (std::is_same_v<T, const char*>) {
                return arg;
            }

            if constexpr (std::is_same_v<T, opentelemetry::nostd::string_view>) {
                return std::string(arg.data(), arg.size());
            }

            return "<unsupported message type>";
        },
        v
    );
}

}  // namespace

namespace wwa::opentelemetry::exporter::logs {

std::shared_ptr<SyslogInterface> SyslogLogRecordExporter::syslog{nullptr};

SyslogLogRecordExporter::SyslogLogRecordExporter(
    ::opentelemetry::nostd::string_view ident, int option, int facility
) noexcept
{
    if (!SyslogLogRecordExporter::syslog) {
        SyslogLogRecordExporter::syslog = std::make_shared<DefaultSyslogImplementation>();
    }

    SyslogLogRecordExporter::syslog->openlog(ident.data(), option, facility);
}

SyslogLogRecordExporter::~SyslogLogRecordExporter()
{
    SyslogLogRecordExporter::syslog->closelog();
}

std::unique_ptr<::opentelemetry::sdk::logs::Recordable> SyslogLogRecordExporter::MakeRecordable() noexcept
{
    return std::make_unique<::opentelemetry::sdk::logs::ReadWriteLogRecord>();
}

// NOLINTNEXTLINE(bugprone-exception-escape)
::opentelemetry::sdk::common::ExportResult SyslogLogRecordExporter::Export(
    const ::opentelemetry::nostd::span<std::unique_ptr<::opentelemetry::sdk::logs::Recordable>>& records
) noexcept
{
    if (this->is_shutdown.load()) {
        return ::opentelemetry::sdk::common::ExportResult::kFailure;
    }

    for (auto& record : records) {
        auto log_record = std::unique_ptr<::opentelemetry::sdk::logs::ReadWriteLogRecord>(
            // NOLINTNEXTLINE(*-type-static-cast-downcast)
            static_cast<::opentelemetry::sdk::logs::ReadWriteLogRecord*>(record.release())
        );

        if (log_record == nullptr) {
            continue;
        }

        auto severity_index = static_cast<std::size_t>(log_record->GetSeverity());
        if (severity_index >= priority_map.size()) {
            severity_index = 0;
        }

        auto priority   = priority_map[severity_index];  // NOLINT(*-bounds-constant-array-index)
        const auto name = log_record->GetInstrumentationScope().GetName();
        const auto body = stringify_attribute(log_record->GetBody());
        const auto msg  = std::string("[").append(name).append("] ").append(body);

        SyslogLogRecordExporter::syslog->syslog(priority, msg);
    }

    return ::opentelemetry::sdk::common::ExportResult::kSuccess;
}

bool SyslogLogRecordExporter::ForceFlush(std::chrono::microseconds) noexcept
{
    return true;
}

bool SyslogLogRecordExporter::Shutdown(std::chrono::microseconds) noexcept
{
    this->is_shutdown.store(true);
    return true;
}

void SyslogLogRecordExporter::setSyslogImplementation(const std::shared_ptr<SyslogInterface>& impl)
{
    if (!impl) {
        SyslogLogRecordExporter::syslog = std::make_shared<DefaultSyslogImplementation>();
    }
    else {
        SyslogLogRecordExporter::syslog = impl;
    }
}

}  // namespace wwa::opentelemetry::exporter::logs
