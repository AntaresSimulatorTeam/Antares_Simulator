add_executable(matrix-benchmark matrix-benchmark.cpp)

target_compile_features(matrix-benchmark PRIVATE cxx_std_20)
target_link_libraries(matrix-benchmark PRIVATE Antares::array Antares::logs)

set_target_properties(matrix-benchmark PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/benchmarks)
