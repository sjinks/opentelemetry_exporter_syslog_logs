#include <opentelemetry/exporters/wwa/syslog/log_record_exporter_factory.h>
#include <opentelemetry/logs/provider.h>
#include <opentelemetry/sdk/logs/logger_provider.h>
#include <opentelemetry/sdk/logs/simple_log_record_processor.h>

namespace {

void init_logging()
{
    auto exporter = wwa::opentelemetry::exporter::logs::SyslogLogRecordExporterFactory::Create("syslog-identifier");

    auto processor = std::make_unique<opentelemetry::sdk::logs::SimpleLogRecordProcessor>(std::move(exporter));

    auto provider = std::make_shared<opentelemetry::sdk::logs::LoggerProvider>(std::move(processor));

    opentelemetry::logs::Provider::SetLoggerProvider(
        std::static_pointer_cast<opentelemetry::logs::LoggerProvider>(provider)
    );
}

}  // namespace

int main()
{
    init_logging();
    opentelemetry::logs::Provider::GetLoggerProvider()->GetLogger("logger name", "somelib")->Info("test message");
    return 0;
}
