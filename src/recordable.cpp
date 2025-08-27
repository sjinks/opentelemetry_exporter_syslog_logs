#include "recordable.h"

#include <array>
#include <cassert>
#include <cstddef>
#include <numeric>
#include <type_traits>

#include <opentelemetry/logs/severity.h>
#include <opentelemetry/nostd/span.h>
#include <opentelemetry/nostd/string_view.h>
#include <opentelemetry/nostd/variant.h>
#include <opentelemetry/trace/span_id.h>
#include <opentelemetry/trace/trace_id.h>
#include <syslog.h>

namespace {

const std::array<int, std::extent_v<decltype(opentelemetry::logs::SeverityNumToText)>> priority_map{
    LOG_ERR,                                             // INVALID
    LOG_DEBUG,   LOG_DEBUG,   LOG_DEBUG,   LOG_DEBUG,    // TRACE, TRACE2, TRACE3, TRACE4
    LOG_DEBUG,   LOG_DEBUG,   LOG_DEBUG,   LOG_DEBUG,    // DEBUG, DEBUG2, DEBUG3, DEBUG4
    LOG_INFO,    LOG_INFO,    LOG_NOTICE,  LOG_NOTICE,   // INFO, INFO2, INFO3, INFO4
    LOG_WARNING, LOG_WARNING, LOG_WARNING, LOG_WARNING,  // WARN, WARN2, WARN3, WARN4
    LOG_ERR,     LOG_ERR,     LOG_CRIT,    LOG_ALERT,    // ERROR, ERROR2, ERROR3, ERROR4
    LOG_EMERG,   LOG_EMERG,   LOG_EMERG,   LOG_EMERG     // FATAL, FATAL2, FATAL3, FATAL4
};

struct stringifier {
    template<typename T>
    std::string operator()(const T& arg) const
    {
        return std::to_string(arg);
    }

    std::string operator()(bool arg) const { return arg ? "true" : "false"; }

    std::string operator()(const char* arg) const { return arg; }
    std::string operator()(const opentelemetry::nostd::string_view& arg) const { return {arg.data(), arg.size()}; }

    template<typename T>
    std::string operator()(const opentelemetry::nostd::span<T>& data) const
    {
        stringifier str;
        std::vector<std::string> strings;
        strings.reserve(data.size());
        for (const auto& item : data) {
            strings.push_back(str(item));
        }

        return !data.empty() ? std::accumulate(
                                   std::next(data.begin()), data.end(), str(*data.begin()),
                                   [&str](const std::string& a, const T& b) { return a + ", " + str(b); }
                               )
                             : std::string{};
    }
};

}  // namespace

namespace wwa::opentelemetry::exporter::logs {

void Recordable::SetTimestamp(::opentelemetry::common::SystemTimestamp) noexcept
{
    // We don't use timestamp
}

void Recordable::SetObservedTimestamp(::opentelemetry::common::SystemTimestamp) noexcept
{
    // We don't use observed timestamp
}

void Recordable::SetSeverity(::opentelemetry::logs::Severity severity) noexcept
{
    auto severity_index = static_cast<std::size_t>(severity);
    if (severity_index >= priority_map.size()) {
        severity_index = 0;
    }

    assert(severity_index < priority_map.size());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    this->m_severity = priority_map[severity_index];
}

void Recordable::SetBody(const ::opentelemetry::common::AttributeValue& message) noexcept
{
    this->m_body = ::opentelemetry::nostd::visit(stringifier{}, message);
}

void Recordable::SetAttribute(
    ::opentelemetry::nostd::string_view, const ::opentelemetry::common::AttributeValue&
) noexcept
{
    // We don't use attributes
}

void Recordable::SetEventId(std::int64_t id, ::opentelemetry::nostd::string_view name) noexcept
{
    this->m_event_id   = id;
    this->m_event_name = std::string(name.data(), name.size());
}

void Recordable::SetTraceId(const ::opentelemetry::trace::TraceId& trace_id) noexcept
{
    if (!this->m_trace_state) {
        this->m_trace_state = std::make_unique<TraceState>();
    }

    this->m_trace_state->trace_id = trace_id;
}

void Recordable::SetSpanId(const ::opentelemetry::trace::SpanId& span_id) noexcept
{
    if (!this->m_trace_state) {
        this->m_trace_state = std::make_unique<TraceState>();
    }

    this->m_trace_state->span_id = span_id;
}

void Recordable::SetTraceFlags(const ::opentelemetry::trace::TraceFlags&) noexcept
{
    // We don't use trace flags
}

void Recordable::SetResource(const ::opentelemetry::sdk::resource::Resource&) noexcept
{
    // We don't use resource
}

void Recordable::SetInstrumentationScope(
    const ::opentelemetry::sdk::instrumentationscope::InstrumentationScope& instrumentation_scope
) noexcept
{
    this->m_instrumentation_scope = &instrumentation_scope;
}

int Recordable::GetSeverity() const noexcept
{
    return this->m_severity;
}

const std::string& Recordable::GetBody() const noexcept
{
    return this->m_body;
}

std::string Recordable::GetEvent() const
{
    if (this->m_event_id != 0) {
        if (this->m_event_name.empty()) {
            return std::to_string(this->m_event_id);
        }

        std::string result = this->m_event_name;
        return result.append(" (").append(std::to_string(this->m_event_id)).append(")");
    }

    return {};
}

std::string Recordable::GetTraceInfo() const
{
    std::string result;
    if (this->m_trace_state) {
        if (this->m_trace_state->trace_id.IsValid()) {
            std::string tid(static_cast<std::string::size_type>(::opentelemetry::trace::TraceId::kSize) * 2U, ' ');
            this->m_trace_state->trace_id.ToLowerBase16(tid);
            result.append("TID: ").append(tid);
        }

        if (this->m_trace_state->span_id.IsValid()) {
            std::string sid(static_cast<std::string::size_type>(::opentelemetry::trace::SpanId::kSize) * 2U, ' ');
            this->m_trace_state->span_id.ToLowerBase16(sid);
            if (!result.empty()) {
                result.append(", ");
            }

            result.append("SID: ").append(sid);
        }
    }

    return result;
}

const ::opentelemetry::sdk::instrumentationscope::InstrumentationScope*
Recordable::GetInstrumentationScope() const noexcept
{
    return this->m_instrumentation_scope;
}

}  // namespace wwa::opentelemetry::exporter::logs
