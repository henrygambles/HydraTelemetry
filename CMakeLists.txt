cmake_minimum_required(VERSION 3.12)
project(HydraTelemetry LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Dependencies
find_package(Protobuf REQUIRED)
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(Threads REQUIRED)

execute_process(
    COMMAND ${Python_EXECUTABLE} -c "import pybind11; print(pybind11.get_cmake_dir())"
    OUTPUT_VARIABLE pybind11_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
find_package(pybind11 REQUIRED)

# Generate C++ files from Protobuf specifications
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS telemetry_record.proto)

# 1. Define the Python Extension Module
pybind11_add_module(hydra_telemetry bindings.cpp ${PROTO_SRCS})
target_include_directories(hydra_telemetry PRIVATE ${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
# Link Protobuf's entire target tree automatically
target_link_libraries(hydra_telemetry PRIVATE protobuf::libprotobuf)

set_target_properties(hydra_telemetry PROPERTIES
    PREFIX ""
    SUFFIX ".so"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# 2. Standalone C++ Flume Pipeline binary
add_executable(flume_engine flume_pipeline.cpp ${PROTO_SRCS})
target_include_directories(flume_engine PRIVATE ${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
# Linking via protobuf::libprotobuf forces CMake to unpack and bind all linked Abseil symbols
target_link_libraries(flume_engine PRIVATE protobuf::libprotobuf Threads::Threads)
