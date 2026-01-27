add_subdirectory(loadFiles)
add_subdirectory(checks)

include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test-modeler-core
        SRC
        testModelerCore.cpp
        LIBS
        Antares::modeler-lib
        Antares::modeler-parameters
        test_utils_unit
)

add_boost_test(test-file-writer
        SRC
        testFileWriter.cpp
        LIBS
        Antares::modeler-lib
        Antares::linear-problem-mpsolver-impl
        Antares::linear-problem-data-impl
        test_utils_unit
)

add_boost_test(test-file-loader
        SRC
        testFileLoader.cpp
        LIBS
        Antares::load-modeler-files
        Antares::modeler-lib
        test_utils_unit
)

add_boost_test(test-parse-modeler-parameters
        SRC
        testParseModelerParameters.cpp
        LIBS
        Antares::modeler-parameters
        test_utils_unit
)

add_boost_test(test-handle-errors
        SRC
        testHandleErrors.cpp
        LIBS
        Antares::load-modeler-files
        test_utils_unit
)

add_boost_test(test-modeler-run
        SRC
        testModelerRun.cpp
        LIBS
        Antares::modeler-lib
        test_utils_unit
)

add_library(mock-modeler-objects
        mockModelerObjects.cpp
        mockModelerObjects.h
        UtilMocks.h
        )

target_link_libraries(mock-modeler-objects
    PUBLIC
  Antares::model-converter
  Antares::antares-study-system-model
        expressions
        expressions-iterators
        linear-problem-mpsolver-impl
    )

target_include_directories(
        mock-modeler-objects
                PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

set_target_properties(mock-modeler-objects PROPERTIES FOLDER Unit-tests)
