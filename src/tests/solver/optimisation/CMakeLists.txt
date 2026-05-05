include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(tests-post-process
        SRC test-post-process.cpp
        LIBS
        Antares::tests::in-memory-study
        model_antares)

add_boost_test(tests-clone-probleme-hebdo
        SRC test-clone-probleme-hebdo.cpp
        LIBS
        Antares::tests::in-memory-study
        antares-solver-simulation
        model_antares)
