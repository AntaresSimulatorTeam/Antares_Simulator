include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(optimConfig
        SRC
        optimConfigBuilder.cpp
        LIBS
        Antares::modeler-optim-config
        test_utils_unit
)
