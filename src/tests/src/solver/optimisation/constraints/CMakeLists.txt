include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(constraints_builder
        SRC
        constraints_builder.cpp
        LIBS
        model_antares
        antares-solver-simulation)
