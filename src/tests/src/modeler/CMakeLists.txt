add_subdirectory(loadFiles)

add_library(mock-modeler-objects
        mockModelerObjects.cpp
        mockModelerObjects.h
        )

target_link_libraries(mock-modeler-objects
    PUBLIC
  Antares::model-converter
  Antares::antares-study-system-model
        expressions
        expressions-iterators
    )

target_include_directories(
        mock-modeler-objects
                PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

set_target_properties(mock-modeler-objects PROPERTIES FOLDER Unit-tests)

