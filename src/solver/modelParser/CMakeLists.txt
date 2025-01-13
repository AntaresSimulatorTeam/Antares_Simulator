set(SOURCES
        parser.cpp
        encoders.hxx
        include/antares/solver/modelParser/parser.h
)

# Create the library
add_library(modelParser STATIC ${SOURCES})
add_library(Antares::modelParser ALIAS modelParser)

# Specify include directories
target_include_directories(modelParser
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Link dependencies (if any)
target_link_libraries(modelParser
        PRIVATE
        yaml-cpp::yaml-cpp
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
