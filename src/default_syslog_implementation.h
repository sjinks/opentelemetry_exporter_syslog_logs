#ifndef D17D59F2_0FDF_4FF4_96EE_244866D19190
#define D17D59F2_0FDF_4FF4_96EE_244866D19190

#include "opentelemetry_exporter_syslog_logs/syslog_interface.h"

class DefaultSyslogImplementation : public opentelemetry::exporter::logs::SyslogInterface {
public:
    void openlog(opentelemetry::nostd::string_view ident, int option, int facility) override;
    void syslog(int priority, opentelemetry::nostd::string_view message) override;
    void closelog() override;
};

#endif /* D17D59F2_0FDF_4FF4_96EE_244866D19190 */
