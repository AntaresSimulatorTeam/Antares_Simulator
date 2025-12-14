set(SOURCES
        checkLocation.cpp

        include/antares/solver/modeler/checks/checkLocation.h
)

# Create the library
add_library(modeler-checks ${SOURCES})
add_library(Antares::modeler-checks ALIAS modeler-checks)

# Specify include directories
target_include_directories(modeler-checks
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

find_package(fmt REQUIRED)
# Link dependencies (if any)
target_link_libraries(modeler-checks
        PUBLIC
        Antares::antares-study-system-model
        PRIVATE
        modeler-lib
        fmt::fmt
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
