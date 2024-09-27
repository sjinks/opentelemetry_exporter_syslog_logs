#ifndef F6CC3A7F_2A2D_4616_B15D_A7739D98F053
#define F6CC3A7F_2A2D_4616_B15D_A7739D98F053

#include <opentelemetry/nostd/string_view.h>
#include "wwa_opentelemetry_exporter_syslog_logs_export.h"

namespace wwa::opentelemetry::exporter::logs {

// NOLINTNEXTLINE(*-special-member-functions)
class WWA_OPENTELEMETRY_EXPORTER_SYSLOG_LOGS_EXPORT SyslogInterface {
public:
    virtual ~SyslogInterface();
    virtual void openlog(::opentelemetry::nostd::string_view ident, int option, int facility) = 0;
    virtual void syslog(int priority, ::opentelemetry::nostd::string_view message)            = 0;
    virtual void closelog()                                                                   = 0;
};

}  // namespace wwa::opentelemetry::exporter::logs

#endif /* F6CC3A7F_2A2D_4616_B15D_A7739D98F053 */
