#include "log_record_exporter.h"

#include <string>

#include <opentelemetry/common/attribute_value.h>
#include <opentelemetry/sdk/instrumentationscope/instrumentation_scope.h>
#include <opentelemetry/sdk/logs/read_write_log_record.h>

#include "default_syslog_implementation.h"
#include "recordable.h"

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
    return std::make_unique<Recordable>();
}

::opentelemetry::sdk::common::ExportResult SyslogLogRecordExporter::Export(
    const ::opentelemetry::nostd::span<std::unique_ptr<::opentelemetry::sdk::logs::Recordable>>& records
) noexcept
{
    if (this->is_shutdown.load()) {
        return ::opentelemetry::sdk::common::ExportResult::kFailure;
    }

    for (const auto& record : records) {
#if defined(__GXX_RTTI) || defined(__cpp_rtti) && __cpp_rtti >= 199711L
        const auto* log_record = dynamic_cast<Recordable*>(record.get());
#else
        const auto* log_record = static_cast<Recordable*>(record.get());
#endif

        if (log_record != nullptr) {
            SyslogLogRecordExporter::process_record(log_record);
        }
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

void SyslogLogRecordExporter::process_record(const Recordable* record)
{
    const auto priority = record->GetSeverity();
    const auto* scope   = record->GetInstrumentationScope();
    const auto& body    = record->GetBody();
    const auto event    = record->GetEvent();
    const auto trace    = record->GetTraceInfo();

    std::string prefix;
    std::string suffix;
    if (scope != nullptr) {
        prefix.append("[").append(scope->GetName());
        if (const auto& version = scope->GetVersion(); !version.empty()) {
            prefix.append("/").append(version);
        }

        prefix.append("] ");
    }

    if (!event.empty() || !trace.empty()) {
        suffix.append(" [");
        if (!event.empty()) {
            suffix.append("EV: ").append(event);
        }

        if (!trace.empty()) {
            if (!event.empty()) {
                suffix.append(", ");
            }

            suffix.append(trace);
        }

        suffix.append("]");
    }

    prefix.append(body).append(suffix);

    SyslogLogRecordExporter::syslog->syslog(priority, prefix);
}

}  // namespace wwa::opentelemetry::exporter::logs
