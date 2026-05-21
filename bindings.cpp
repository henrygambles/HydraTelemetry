#include <pybind11/pybind11.h>
#include "telemetry_agent.hpp"

namespace py = pybind11;

PYBIND11_MODULE(hydra_telemetry, m) {
    py::enum_<telemetry::Framework>(m, "Framework")
        .value("UNKNOWN", telemetry::Framework::FRAMEWORK_UNSPECIFIED)
        .value("PYTORCH", telemetry::Framework::PYTORCH)
        .value("JAX", telemetry::Framework::JAX);

    py::enum_<telemetry::MetricType>(m, "MetricType")
        .value("OP_DURATION", telemetry::MetricType::OP_DURATION_US)
        .value("MEMORY", telemetry::MetricType::MEMORY_ALLOCATION_BYTES)
        .value("COMM_DELAY", telemetry::MetricType::COMMUNICATION_DELAY_US);

    py::class_<TelemetryAgent>(m, "TelemetryAgent")
        .def(py::init<const std::string&, telemetry::Framework, size_t>(),
             py::arg("node_id"), py::arg("framework"), py::arg("batch_threshold") = 1000)
        .def("record_event", &TelemetryAgent::record_event);
}
