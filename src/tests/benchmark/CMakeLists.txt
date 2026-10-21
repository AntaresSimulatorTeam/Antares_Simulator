add_executable(matrix-benchmark matrix-benchmark.cpp)
add_executable(matrix-load-benchmark matrix-load-benchmark.cpp)

target_compile_features(matrix-benchmark PRIVATE cxx_std_20)
target_compile_features(matrix-load-benchmark PRIVATE cxx_std_20)
target_link_libraries(matrix-benchmark PRIVATE Antares::array)
target_link_libraries(matrix-load-benchmark PRIVATE Antares::array)

set_target_properties(matrix-benchmark PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/benchmarks)
set_target_properties(matrix-load-benchmark PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/benchmarks)
