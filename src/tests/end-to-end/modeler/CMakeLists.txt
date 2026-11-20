include(${CMAKE_SOURCE_DIR}/src/tests/macros.cmake)

add_boost_test(tests-modeler
        SRC test-modeler.cpp
        LIBS
        Antares::tests::in-memory-study
        Antares::logs
        Antares::modeler-lib
        Antares::load-modeler-files
        Antares::tests::inmemory-modeler
)

target_compile_definitions(tests-modeler PRIVATE
        RESOURCES_DIR="${CMAKE_SOURCE_DIR}/resources")
