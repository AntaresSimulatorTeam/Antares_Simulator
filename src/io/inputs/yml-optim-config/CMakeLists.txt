set(SOURCES
        parser.cpp
        converter.cpp
        decoders.hxx
        include/antares/io/inputs/yml-optim-config/parser.h
)

# Create the library
add_library(yml-optim-config ${SOURCES})
add_library(Antares::yml-optim-config ALIAS yml-optim-config)

# Specify include directories
target_include_directories(yml-optim-config
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Link dependencies (if any)
target_link_libraries(yml-optim-config
        PRIVATE
        yaml-cpp::yaml-cpp
        Antares::exception
        PUBLIC
        Antares::antares-study-system-model
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
