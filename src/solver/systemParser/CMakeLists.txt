find_package(yaml-cpp REQUIRED)

set(SOURCES
        parser.cpp
        encoders.hxx
        include/antares/solver/systemParser/parser.h
)

# Create the library
add_library(systemParser STATIC ${SOURCES})
add_library(Antares::systemParser ALIAS systemParser)

# Specify include directories
target_include_directories(systemParser
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Link dependencies (if any)
target_link_libraries(systemParser
        PRIVATE
        yaml-cpp
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
