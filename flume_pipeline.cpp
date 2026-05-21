#include <iostream>
#include <vector>
#include <unordered_map>
#include <thread>
#include <shared_mutex>
#include <fstream>
#include "telemetry_record.pb.h"

struct AggregatedMetric {
    double total_value = 0.0;
    int count = 0;
};

// Simple parallel execution coordinator mimicking Flume's Map & Combine phases
class FlumeLitePipeline {
private:
    std::unordered_map<std::string, AggregatedMetric> global_aggregates_;
    std::shared_mutex pipeline_mutex_;

public:
    void process_log_file(const std::string& filepath) {
        std::ifstream input(filepath, std::ios::binary);
        if (!input.is_open()) return;

        // Map Phase: Parse sequential stream records locally
        std::unordered_map<std::string, AggregatedMetric> local_map;

        while (input.peek() != EOF) {
            uint32_t size;
            input.read(reinterpret_cast<char*>(&size), sizeof(size));
            
            telemetry::TelemetryBatch batch;
            std::vector<char> buffer(size);
            input.read(buffer.data(), size);
            batch.ParseFromArray(buffer.data(), size);

            for (const auto& event : batch.events()) {
                if (event.metric_type() == telemetry::MetricType::OP_DURATION_US) {
                    std::string key = event.operator_name() + "_" + event.tensor_shape();
                    local_map[key].total_value += event.metric_value();
                    local_map[key].count++;
                }
            }
        }

        // Combine / Reduce Phase: Merge thread-local maps safely into the pipeline state
        std::unique_lock<std::shared_mutex> lock(pipeline_mutex_);
        for (const auto& [key, metrics] : local_map) {
            global_aggregates_[key].total_value += metrics.total_value;
            global_aggregates_[key].count += metrics.count;
        }
    }

    void export_to_csv(const std::string& out_filename) {
        std::ofstream csv(out_filename);
        csv << "operator_signature,avg_duration_us,execution_count\n";
        std::shared_lock<std::shared_mutex> lock(pipeline_mutex_);
        for (const auto& [key, metric] : global_aggregates_) {
            double avg = metric.total_value / metric.count;
            csv << key << "," << avg << "," << metric.count << "\n";
        }
    }
};

int main() {
    FlumeLitePipeline pipeline;

    // Simulate multi-threaded processing of multiple worker output logs
    std::vector<std::string> log_files = {"worker_node_0_telemetry.bin", "worker_node_1_telemetry.bin"};
    std::vector<std::thread> workers;

    for (const auto& file : log_files) {
        workers.emplace_back(&FlumeLitePipeline::process_log_file, &pipeline, file);
    }

    for (auto& t : workers) {
        t.join();
    }

    pipeline.export_to_csv("pipeline_analytics_output.csv");
    std::cout << "[FlumeLite Engine]: Execution complete. Aggregates exported." << std::endl;
    return 0;
}
