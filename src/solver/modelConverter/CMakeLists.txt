set(SOURCES
        modelConverter.cpp
        convertorVisitor.cpp
        include/antares/solver/modelConverter/modelConverter.h
        include/antares/solver/modelConverter/convertorVisitor.h
)

# Create the library
add_library(modelConverter STATIC ${SOURCES})
add_library(Antares::modelConverter ALIAS modelConverter)

# Specify include directories
target_include_directories(modelConverter
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Link dependencies (if any)
target_link_libraries(modelConverter
        PRIVATE
        Antares::antares-study-system-model
        Antares::modelParser
        Antares::antlr-interface
        Antares::solver-expressions
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
