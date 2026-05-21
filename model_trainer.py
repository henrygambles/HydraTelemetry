import torch
import time
import hydra_telemetry as ht

print("====== [HydraTelemetry Worker Simulation Engine] ======")

# Initialize our native C++ tracking agent 
# We set a low batch threshold of 5 so it forces flushes immediately for our short demo
agent = ht.TelemetryAgent(node_id="worker_node_0", framework=ht.Framework.PYTORCH, batch_threshold=5)

class MonitoredLinear(torch.nn.Module):
    def __init__(self, in_features, out_features):
        super().__init__()
        self.weight = torch.nn.Parameter(torch.randn(out_features, in_features))
        
    def forward(self, x, step_id=0):
        start = time.perf_counter_ns()
        
        # 1. Profile Allocation Memory Overhead
        shape_str = f"{x.shape[0]}x{x.shape[1]}"
        alloc_bytes = x.element_size() * x.nelement()
        agent.record_event(step_id, "Linear_Forward_Alloc", ht.MetricType.MEMORY, float(alloc_bytes), shape_str)
        
        # Core operation execution
        out = torch.matmul(x, self.weight.t())
        
        # 2. Profile Execution Latency Window
        duration = (time.perf_counter_ns() - start) / 1000.0 # to microseconds
        agent.record_event(step_id, "Linear_MatMul_Forward", ht.MetricType.OP_DURATION, duration, shape_str)
        return out

# Initialize network components
layer = MonitoredLinear(in_features=1024, out_features=512)

# Simulate a 10-step training loop with varying tensor input workloads
print("Simulating deep learning training iterations...")
for step in range(1, 11):
    # Alternating tensor batch sizing to simulate realistic production workloads
    batch_size = 64 if step % 2 == 0 else 128
    x_input = torch.randn(batch_size, 1024)
    
    # Forward Pass through our instrumented architecture
    y_output = layer(x_input, step_id=step)
    
    # Introduce tiny variations to mock dynamic execution behavior
    time.sleep(0.02)

print("[Worker]: Loop execution complete. Native logs successfully flushed to disk.")
