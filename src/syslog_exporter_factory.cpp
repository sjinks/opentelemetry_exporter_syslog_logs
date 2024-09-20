#include "opentelemetry_exporter_syslog_logs/syslog_exporter_factory.h"
#include "syslog_exporter.h"

namespace opentelemetry::exporter::logs {

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
SyslogLogRecordExporterFactory::Create(opentelemetry::nostd::string_view ident)
{
    return std::make_unique<SyslogLogRecordExporter>(ident);
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
SyslogLogRecordExporterFactory::Create(opentelemetry::nostd::string_view ident, int option, int facility)
{
    return std::make_unique<SyslogLogRecordExporter>(ident, option, facility);
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> SyslogLogRecordExporterFactory::Create(
    opentelemetry::nostd::string_view ident, const std::shared_ptr<SyslogInterface>& syslog
)
{
    SyslogLogRecordExporter::setSyslogImplementation(syslog);
    return std::make_unique<SyslogLogRecordExporter>(ident);
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> SyslogLogRecordExporterFactory::Create(
    opentelemetry::nostd::string_view ident, const std::shared_ptr<SyslogInterface>& syslog, int option, int facility
)
{
    SyslogLogRecordExporter::setSyslogImplementation(syslog);
    return std::make_unique<SyslogLogRecordExporter>(ident, option, facility);
}

void SyslogLogRecordExporterFactory::setSyslogImplementation(const std::shared_ptr<SyslogInterface>& syslog)
{
    SyslogLogRecordExporter::setSyslogImplementation(syslog);
}

}  // namespace opentelemetry::exporter::logs
