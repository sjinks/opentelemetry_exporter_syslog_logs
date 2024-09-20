#ifndef BB1766CD_2D39_4A14_83BC_75E2A0AE3585
#define BB1766CD_2D39_4A14_83BC_75E2A0AE3585

#include <atomic>
#include <memory>

#include <syslog.h>

#include <opentelemetry/nostd/string_view.h>
#include <opentelemetry/sdk/logs/exporter.h>

#include "opentelemetry_exporter_syslog_logs/syslog_interface.h"

namespace opentelemetry::exporter::logs {

// NOLINTNEXTLINE(*-special-member-functions)
class SyslogLogRecordExporter final : public opentelemetry::sdk::logs::LogRecordExporter {
public:
    explicit SyslogLogRecordExporter(
        opentelemetry::nostd::string_view ident, int option = LOG_CONS | LOG_PID, int facility = LOG_USER
    ) noexcept;

    ~SyslogLogRecordExporter() override;

    std::unique_ptr<opentelemetry::sdk::logs::Recordable> MakeRecordable() noexcept override;

    opentelemetry::sdk::common::ExportResult
    Export(const opentelemetry::nostd::span<std::unique_ptr<opentelemetry::sdk::logs::Recordable>>& records
    ) noexcept override;

    bool ForceFlush(std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override;
    bool Shutdown(std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override;

    static void setSyslogImplementation(const std::shared_ptr<SyslogInterface>& impl);

private:
    std::atomic<bool> is_shutdown{false};
    static std::shared_ptr<SyslogInterface> syslog;
};

}  // namespace opentelemetry::exporter::logs

#endif /* BB1766CD_2D39_4A14_83BC_75E2A0AE3585 */
