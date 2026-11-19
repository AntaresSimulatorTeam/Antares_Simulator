include(${CMAKE_SOURCE_DIR}/src/tests/macros.cmake)

add_boost_test(tests-post-process
        SRC test-post-process.cpp
        LIBS
        Antares::tests::in-memory-study
        model_antares)

add_boost_test(test-hebdo-area-price-provider
        SRC test-hebdo-area-price-provider.cpp
        LIBS
        model_antares
        Antares::linear-problem-mpsolver-impl)
