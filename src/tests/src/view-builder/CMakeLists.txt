include(${CMAKE_SOURCE_DIR}/src/tests/macros.cmake)

add_boost_test(test-view-builder
    SRC test_view_builder.cpp
    LIBS
    Antares::view-builder
    Antares::antares-core
    Antares::modeler-lib
    yaml-cpp::yaml-cpp)
