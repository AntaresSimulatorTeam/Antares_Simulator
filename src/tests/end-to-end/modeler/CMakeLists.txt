include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(tests-modeler
        SRC test-modeler.cpp
        LIBS
        Antares::tests::in-memory-study
        Antares::logs
        Antares::modeler-lib
        Antares::tests::inmemory-modeler
)
