set(SOURCES
        systemConverter.cpp
        include/antares/solver/systemConverter/systemConverter.h
)

# Create the library
add_library(systemConverter STATIC ${SOURCES})
add_library(Antares::systemConverter ALIAS systemConverter)

# Specify include directories
target_include_directories(systemConverter
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Link dependencies (if any)
target_link_libraries(systemConverter
        PRIVATE
        Antares::antares-study-system-model
        Antares::systemParser
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
