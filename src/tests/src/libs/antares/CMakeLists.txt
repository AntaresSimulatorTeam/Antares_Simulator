add_subdirectory(antares)
add_subdirectory(concurrency)
add_subdirectory(writer)
add_subdirectory(study)
add_subdirectory(benchmarking)
add_subdirectory(checks)
add_subdirectory(inifile)
add_subdirectory(scenarioBuilderExpression)

add_subdirectory(yaml-parser)
add_subdirectory(antlr4-interface)

set(src_libs_antares "${CMAKE_SOURCE_DIR}/src/libs/antares")

# Building tests on Matrix save operations
add_boost_test(tests-matrix-save
        SRC
        logs/antares/logs/logs.h
        array/fill-matrix.h
        array/tests-matrix-save.h
        array/tests-matrix-save.cpp
        INCLUDE
        "${src_libs_antares}/array/include"
        "${src_libs_antares}/io/include"
        "${src_libs_antares}/memory/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/logs"
        "${CMAKE_SOURCE_DIR}/src/tests/src/libs"
        LIBS
        Antares::array
        Antares::io
        yuni-static-core
        antares-core
        Antares::exception)

# Building tests on Matrix load operations
add_boost_test(tests-matrix-load
        SRC
        array/fill-matrix.h
        array/tests-matrix-load.h
        array/tests-matrix-load.cpp
        INCLUDE
        "${src_libs_antares}/array/include"
        "${src_libs_antares}/io/include"
        "${src_libs_antares}/memory/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/logs"
        "${CMAKE_SOURCE_DIR}/src/tests/src/libs"
        LIBS
        Antares::array
        Antares::io
        yuni-static-core
        antares-core
        Antares::exception)

# Test utilities
add_boost_test(test-utils
        SRC test_utils.cpp
        LIBS
        Antares::study-utils
        yuni-static-core)

add_boost_test(logs-accumulation
        SRC
	logs/logs-test.cpp
        LIBS
        yuni-static-core
	Antares::logs
	test_utils_unit
)

add_boost_test(logs-format
        SRC
	logs/log-format-test.cpp
        LIBS
	Antares::logs
	test_utils_unit
)
