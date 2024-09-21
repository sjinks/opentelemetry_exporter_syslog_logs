# opentelemetry_exporter_syslog_logs

Exports OpenTelemetry log records to syslog.

## Installation

```shell
cmake -B build
cmake --build build
cmake --install build
```

**Dependencies:** [opentelemetry-cpp](https://github.com/open-telemetry/opentelemetry-cpp) >= 1.11.0

**Development Dependencies:** [Google Test with GMock](https://github.com/google/googletest) (required to build and run tests)

### CMake Options

| Option Name               | Description                              | Default Value |
|---------------------------|------------------------------------------|---------------|
| `WITH_TESTING`            | Whether to enable tests                  | `ON`          |
| `INSTALL_SYSLOG_EXPORTER` | Whether to install the syslog exporter   | `ON`          |

## Usage

```cmake
find_package(opentelemetry_exporter_syslog_logs CONFIG REQUIRED)
target_link_libraries(
    my_target
    PRIVATE
        opentelemetry_exporter_syslog_logs::opentelemetry_exporter_syslog_logs
)
```

```cpp
#include <opentelemetry/logs/provider.h>
#include <opentelemetry/sdk/logs/logger_provider.h>
#include <opentelemetry/sdk/logs/simple_log_record_processor.h>
#include <opentelemetry_exporter_syslog_logs/syslog_exporter_factory.h>

void init_logging()
{
    auto exporter = opentelemetry::exporter::logs::SyslogLogRecordExporterFactory::Create(
        "syslog-identifier"
    );

    auto processor = std::make_unique<opentelemetry::sdk::logs::SimpleLogRecordProcessor>(
        std::move(exporter)
    );

    auto provider = std::make_shared<opentelemetry::sdk::logs::LoggerProvider>(
        std::move(processor)
    );

    opentelemetry::logs::Provider::SetLoggerProvider(
        std::static_pointer_cast<opentelemetry::logs::LoggerProvider>(provider)
    );
}
```
