# opentelemetry_exporter_syslog_logs

[![Build and Test](https://github.com/sjinks/opentelemetry_exporter_syslog_logs/actions/workflows/ci.yml/badge.svg)](https://github.com/sjinks/opentelemetry_exporter_syslog_logs/actions/workflows/ci.yml)
[![CodeQL](https://github.com/sjinks/opentelemetry_exporter_syslog_logs/actions/workflows/codeql.yml/badge.svg)](https://github.com/sjinks/opentelemetry_exporter_syslog_logs/actions/workflows/codeql.yml)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=sjinks_opentelemetry_exporter_syslog_logs&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=sjinks_opentelemetry_exporter_syslog_logs)

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
find_package(wwa_opentelemetry_exporter_syslog_logs CONFIG REQUIRED)
target_link_libraries(
    my_target
    PRIVATE
        wwa::opentelemetry::syslog_log_record_exporter
)
```

```cpp
#include <opentelemetry/logs/provider.h>
#include <opentelemetry/sdk/logs/logger_provider.h>
#include <opentelemetry/sdk/logs/simple_log_record_processor.h>
#include <opentelemetry/exporters/wwa/syslog/log_record_exporter_factory.h>

void init_logging()
{
    auto exporter = wwa::opentelemetry::exporter::logs::SyslogLogRecordExporterFactory::Create(
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

See [ci/CMakeLists.txt](ci/CMakeLists.txt) and [ci/main.cpp](ci/main.cpp) for an example.
