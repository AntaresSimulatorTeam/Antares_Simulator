set(SOURCES
        parser.cpp
        converter.cpp
        encoders.hxx
        include/antares/solver/systemParser/parser.h
        include/antares/solver/systemParser/converter.h
        include/antares/solver/systemParser/system.h
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
        PUBLIC
        Antares::antares-study-system-model
        PRIVATE
        yaml-cpp::yaml-cpp
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
