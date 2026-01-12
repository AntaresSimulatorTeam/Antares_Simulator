include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test-load-yaml-files
        SRC
        testLoadModelerFiles.cpp
        testParameters.cpp
        testLoadScenarioGroup.cpp
        test-load-optim-config.cpp
        LIBS
        Antares::modeler-parameters
        Antares::load-modeler-files
        Antares::modeler-lib
        test_utils_unit
)
