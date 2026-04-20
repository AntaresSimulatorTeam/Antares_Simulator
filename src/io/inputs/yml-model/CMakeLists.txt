set(SOURCES
        parser.cpp
        decoders.cpp
        include/antares/io/inputs/yml-model/decoders.h
        include/antares/io/inputs/yml-model/parser.h
)

# Create the library
add_library(yml-model ${SOURCES})
add_library(Antares::yml-model ALIAS yml-model)

# Specify include directories
target_include_directories(yml-model
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Link dependencies (if any)
target_link_libraries(yml-model
        PUBLIC
        Antares::input-error
        fmt::fmt
        Antares::yml-utils
        PRIVATE
        yaml-cpp::yaml-cpp
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
