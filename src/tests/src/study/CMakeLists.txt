include(${CMAKE_SOURCE_DIR}/src/tests/macros.cmake)

add_boost_test(test-miscgen-names
    SRC test_miscgen_names.cpp
    LIBS
    Antares::antares-core)

add_subdirectory(system-model)