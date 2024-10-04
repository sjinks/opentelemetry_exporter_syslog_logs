#ifndef CF4E6AE0_CF35_4093_B565_FBC287F3EEBE
#define CF4E6AE0_CF35_4093_B565_FBC287F3EEBE

#ifdef WWA_OPENTELEMETRY_EXPORTER_SYSLOG_LOGS_STATIC_DEFINE
#    define WWA_OPENTELEMETRY_EXPORTER_SYSLOG_LOGS_EXPORT
#    define WWA_OPENTELEMETRY_EXPORTER_SYSLOG_LOGS_NO_EXPORT
#else
#    ifdef wwa_opentelemetry_exporter_syslog_logs_EXPORTS
/* We are building this library; export */
#        if defined _WIN32 || defined __CYGWIN__
#            define WWA_OPENTELEMETRY_EXPORTER_SYSLOG_LOGS_EXPORT __declspec(dllexport)
#            define WWA_OPENTELEMETRY_EXPORTER_SYSLOG_LOGS_NO_EXPORT
#        else
#            define WWA_OPENTELEMETRY_EXPORTER_SYSLOG_LOGS_EXPORT    [[gnu::visibility("default")]]
#            define WWA_OPENTELEMETRY_EXPORTER_SYSLOG_LOGS_NO_EXPORT [[gnu::visibility("hidden")]]
#        endif
#    else
/* We are using this library; import */
#        if defined _WIN32 || defined __CYGWIN__
#            define WWA_OPENTELEMETRY_EXPORTER_SYSLOG_LOGS_EXPORT __declspec(dllimport)
#            define WWA_OPENTELEMETRY_EXPORTER_SYSLOG_LOGS_NO_EXPORT
#        else
#            define WWA_OPENTELEMETRY_EXPORTER_SYSLOG_LOGS_EXPORT    [[gnu::visibility("default")]]
#            define WWA_OPENTELEMETRY_EXPORTER_SYSLOG_LOGS_NO_EXPORT [[gnu::visibility("hidden")]]
#        endif
#    endif
#endif

#endif /* CF4E6AE0_CF35_4093_B565_FBC287F3EEBE */
