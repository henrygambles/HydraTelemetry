# 🐲 HydraTelemetry 🐲
An End-to-End, Low-Overhead ML Performance Profiling & Telemetry Pipeline.

Built with C++, Python, Protocol Buffers, and DuckDB to diagnose cluster inefficiencies at scale. 🚀
```
  [ ML Training Worker ] 
         │  (Low-Overhead C++ Agent Instrumentation)
         ▼ 
  [ Zero-Allocation .bin Logs ] ──( Protobuf Streaming )──► [ FlumeLite Engine ]
                                                                   │ (MapReduce Aggregations)
                                                                   ▼
  [ Tabular Analytics Studio ] ◄──( High-Performance SQL )── [ DuckDB Columns ]
```
## 🎭 The Reason
Why build this? Because inside multi-billion dollar ML training clusters, even a 2% latency regression or an unaligned XLA tensor layout can burn millions of dollars in compute. 💸

HydraTelemetry is a multi-language telemetry rig engineered to sniff out cluster bottlenecks without slowing down the core workload. It mimics the internal design patterns used at warehouse-scale (like Google's Flume and Dremel) by decoupling lightning-fast native data collection from heavy offline analytical processing.

⚡ Key Architectural Superpowers
🏎️ Sub-Microsecond C++ Instrumentation Agent: Core tracking loops are written in bare-metal C++ with thread-safe, double-buffered file streams to keep training loop regression near zero.

🧬 Structural Serialization via Protobuf: Uses structured Protocol Buffers for zero-copy binary streaming data contracts. No sloppy string parsing allowed here! 🙅‍♂️

🛠️ Pybind11 Runtime Glue: Exposes the underlying C++ tracking module as a native Python extension, allowing researchers to drop hooks straight into PyTorch or JAX.

🧹 Decoupled FlumeLite MapReduce Aggregator: Batch-processes high-throughput worker binaries using map-reduce paradigms to isolate pipeline bottlenecks by specific operator signatures and dimensions.

📊 Relational Observability with DuckDB: Runs lightning-fast analytical SQL queries directly on pipeline outputs to instantly isolate cluster stragglers or XLA re-compilation penalties. 🕵️‍♂️

## 📂 Repository Blueprint
```
HydraTelemetry/
├── CMakeLists.txt              # Build architecture linking Protobuf & Pybind11
├── telemetry_record.proto      # The single source of truth for tracking schemas
├── telemetry_agent.hpp         # High-performance C++ agent (asynchronous disk buffer)
├── bindings.cpp                # Python compilation bridges via pybind11
├── model_trainer.py            # Deep learning workload simulator (PyTorch hooks)
├── flume_engine.py             # MapReduce batch aggregator (Decompression & Reduce)
└── analytics.sql               # Production-grade database analysis scripts
```
## 🚀 Let's Run It! (Step-by-Step)
### 📦 1. Spin up the Environment Dependencies
Ensure your terminal environment has the required compilers, build bridges, and analytics parsing utilities ready to rock:

Get the native tools via Homebrew:
```
brew install cmake protobuf duckdb
```
Grab the Python runtime extensions:
```
pip install pybind11 duckdb protobuf
```
### 🔨 2. Compile the Native Engine
Run CMake to build the cross-language bridges. This links your local system compiler directly into your runtime workspace:

```
mkdir build && cd build
cmake -DPython_EXECUTABLE=$(which python) ..
make -j4
cd ..
(This auto-generates your high-performance hydra_telemetry.so module!)
```

### 🧬 3. Generate the Protocol Buffer Bridges
Compile your system schema into Python-readable generation code:

```
protoc --python_out=. telemetry_record.proto
```
🏃‍♂️ 4. Execute the End-to-End Pipeline Loop
Now, trigger the complete system lifecycle loop from tracking to database analysis:

Step A: Run the training simulator to dump performance logs via C++ hooks
python model_trainer.py

Step B: Run the Flume processing engine to unpack and merge the binaries
python flume_engine.py

Step C: Spin up the SQL Analytics Engine to visualize performance results!
```
duckdb -c "SELECT * FROM read_csv_auto('pipeline_analytics_output.csv') ORDER BY avg_duration_us DESC;"
```
📈 Sample Production Insights
When the pipeline runs successfully, your infrastructure team gets a beautifully isolated signature matrix showing exact computation variations based on tensor layouts:
```
┌────────────────────────────────┬────────────────────┬─────────────────┐
│        operator_signature      │  avg_duration_us   │ execution_count │
│            varchar             │       double       │      int64      │
├────────────────────────────────┼────────────────────┼─────────────────┤
│ Linear_MatMul_Forward_128x1024 │ 1346.6499999999999 │               5 │
│ Linear_MatMul_Forward_64x1024  │           432.1088 │               5 │
└────────────────────────────────┴────────────────────┴─────────────────┘
```
