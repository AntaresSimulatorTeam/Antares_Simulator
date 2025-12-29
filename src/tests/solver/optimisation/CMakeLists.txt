include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(tests-post-process
        SRC test-post-process.cpp
        LIBS
        Antares::tests::in-memory-study
        model_antares)
