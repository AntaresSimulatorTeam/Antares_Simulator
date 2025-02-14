set(SOURCES
        parser.cpp
        decoders.hxx
        include/antares/io/inputs/yml-model/parser.h
)

# Create the library
add_library(yml-model STATIC ${SOURCES})
add_library(Antares::yml-model ALIAS yml-model)

# Specify include directories
target_include_directories(yml-model
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Link dependencies (if any)
target_link_libraries(yml-model
        PRIVATE
        yaml-cpp::yaml-cpp
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
