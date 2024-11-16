#ifndef C3F17F64_9C46_4BB6_8F0D_939E493F1173
#define C3F17F64_9C46_4BB6_8F0D_939E493F1173

#include <cstdint>
#include <string>

#include <opentelemetry/common/attribute_value.h>
#include <opentelemetry/common/timestamp.h>
#include <opentelemetry/logs/severity.h>
#include <opentelemetry/nostd/string_view.h>
#include <opentelemetry/sdk/common/attribute_utils.h>
#include <opentelemetry/sdk/instrumentationscope/instrumentation_scope.h>
#include <opentelemetry/sdk/logs/recordable.h>
#include <opentelemetry/sdk/resource/resource.h>
#include <opentelemetry/trace/span_id.h>
#include <opentelemetry/trace/trace_flags.h>
#include <opentelemetry/trace/trace_id.h>

namespace wwa::opentelemetry::exporter::logs {

class Recordable final : public ::opentelemetry::sdk::logs::Recordable {
public:
    /**
     * Set the timestamp for this log.
     * @param timestamp the timestamp to set
     */
    void SetTimestamp(::opentelemetry::common::SystemTimestamp timestamp) noexcept override;

    /**
     * Set the observed timestamp for this log.
     * @param timestamp the timestamp to set
     */
    void SetObservedTimestamp(::opentelemetry::common::SystemTimestamp timestamp) noexcept override;

    /**
     * Set the severity for this log.
     * @param severity the severity of the event
     */
    void SetSeverity(::opentelemetry::logs::Severity severity) noexcept override;

    /**
     * Set body field for this log.
     * @param message the body to set
     */
    void SetBody(const ::opentelemetry::common::AttributeValue& message) noexcept override;

    /**
     * Set an attribute of a log.
     * @param key the name of the attribute
     * @param value the attribute value
     */
    void SetAttribute(
        ::opentelemetry::nostd::string_view key, const ::opentelemetry::common::AttributeValue& value
    ) noexcept override;

    /**
     * Set the Event Id.
     * @param id The event id to set
     * @param name Optional event name to set
     */
    void SetEventId(std::int64_t id, ::opentelemetry::nostd::string_view name = {}) noexcept override;

    /**
     * Set the trace id for this log.
     * @param trace_id the trace id to set
     */
    void SetTraceId(const ::opentelemetry::trace::TraceId& trace_id) noexcept override;

    /**
     * Set the span id for this log.
     * @param span_id the span id to set
     */
    void SetSpanId(const ::opentelemetry::trace::SpanId& span_id) noexcept override;

    /**
     * Inject trace_flags for this log.
     * @param trace_flags the trace flags to set
     */
    void SetTraceFlags(const ::opentelemetry::trace::TraceFlags& trace_flags) noexcept override;

    /**
     * Set Resource of this log
     * @param Resource the resource to set
     */
    void SetResource(const ::opentelemetry::sdk::resource::Resource& resource) noexcept override;

    /**
     * Set instrumentation_scope for this log.
     * @param instrumentation_scope the instrumentation scope to set
     */
    void SetInstrumentationScope(
        const ::opentelemetry::sdk::instrumentationscope::InstrumentationScope& instrumentation_scope
    ) noexcept override;

    [[nodiscard]] int GetSeverity() const noexcept;
    [[nodiscard]] const std::string& GetBody() const noexcept;
    [[nodiscard]] std::string GetEvent() const;
    [[nodiscard]] std::string GetTraceInfo() const;
    [[nodiscard]] const ::opentelemetry::sdk::instrumentationscope::InstrumentationScope*
    GetInstrumentationScope() const noexcept;

private:
    const ::opentelemetry::sdk::instrumentationscope::InstrumentationScope* m_instrumentation_scope = nullptr;

    std::string m_body;
    std::string m_event_name;

    struct TraceState {
        ::opentelemetry::trace::TraceId trace_id;
        ::opentelemetry::trace::SpanId span_id;
    };

    std::unique_ptr<TraceState> m_trace_state;

    std::int64_t m_event_id = 0;
    int m_severity;
};

}  // namespace wwa::opentelemetry::exporter::logs

#endif /* C3F17F64_9C46_4BB6_8F0D_939E493F1173 */
