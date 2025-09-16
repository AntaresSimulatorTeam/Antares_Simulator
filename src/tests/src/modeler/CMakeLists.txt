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
