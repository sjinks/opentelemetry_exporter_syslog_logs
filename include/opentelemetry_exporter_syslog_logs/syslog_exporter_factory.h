#ifndef FD634219_63FB_4E23_BC1D_6DB0BC20B6BE
#define FD634219_63FB_4E23_BC1D_6DB0BC20B6BE

#include <memory>
#include <opentelemetry/nostd/string_view.h>
#include <opentelemetry/sdk/logs/exporter.h>

#include "opentelemetry_exporter_syslog_logs_export.h"
#include "syslog_interface.h"

namespace opentelemetry::exporter::logs {

class OPENTELEMETRY_EXPORTER_SYSLOG_LOGS_EXPORT SyslogLogRecordExporterFactory {
public:
    static std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Create(opentelemetry::nostd::string_view ident);

    static std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
    Create(opentelemetry::nostd::string_view ident, int option, int facility);

    static std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
    Create(opentelemetry::nostd::string_view ident, const std::shared_ptr<SyslogInterface>& syslog);

    static std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Create(
        opentelemetry::nostd::string_view ident, const std::shared_ptr<SyslogInterface>& syslog, int option,
        int facility
    );

    static void setSyslogImplementation(const std::shared_ptr<SyslogInterface>& syslog);
};

}  // namespace opentelemetry::exporter::logs

#endif /* FD634219_63FB_4E23_BC1D_6DB0BC20B6BE */
