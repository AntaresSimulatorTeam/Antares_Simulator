# TODO: merge this with yml-model?

set(SOURCES
        modelConverter.cpp
        convertorVisitor.cpp
        include/antares/io/inputs/model-converter/modelConverter.h
        include/antares/io/inputs/model-converter/convertorVisitor.h
)

# Create the library
add_library(model-converter STATIC ${SOURCES})
add_library(Antares::model-converter ALIAS model-converter)

# Specify include directories
target_include_directories(model-converter
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Link dependencies (if any)
target_link_libraries(model-converter
        PRIVATE
        Antares::antares-study-system-model
        Antares::yml-model
        Antares::antlr-interface
        Antares::expressions
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
