#include "default_syslog_implementation.h"

#include <syslog.h>

void DefaultSyslogImplementation::openlog(opentelemetry::nostd::string_view ident, int option, int facility)
{
    ::openlog(ident.data(), option, facility);
}

void DefaultSyslogImplementation::syslog(int priority, opentelemetry::nostd::string_view message)
{
    ::syslog(priority, "%s", message.data());  // NOLINT(*-type-vararg)
}

void DefaultSyslogImplementation::closelog()
{
    ::closelog();
}
