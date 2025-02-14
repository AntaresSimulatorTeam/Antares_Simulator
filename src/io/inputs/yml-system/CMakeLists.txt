set(SOURCES
        parser.cpp
        converter.cpp
        decoders.hxx
        include/antares/io/inputs/yml-system/parser.h
        include/antares/io/inputs/yml-system/converter.h
        include/antares/io/inputs/yml-system/system.h
)

# Create the library
add_library(yml-system STATIC ${SOURCES})
add_library(Antares::yml-system ALIAS yml-system)

# Specify include directories
target_include_directories(yml-system
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Link dependencies (if any)
target_link_libraries(yml-system
        PUBLIC
        Antares::antares-study-system-model
        PRIVATE
        yaml-cpp::yaml-cpp
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
