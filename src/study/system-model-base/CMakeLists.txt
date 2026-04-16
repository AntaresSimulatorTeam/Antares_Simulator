project(system-model-base)

set(SRC
        include/antares/study/system-model-base/variabilityType.h
)

# This is a header-only (interface) library: declare it as INTERFACE so
# CMake does not try to determine a linker language from headers.
add_library(system-model-base INTERFACE)

add_library(Antares::system-model-base ALIAS system-model-base)

target_include_directories(system-model-base
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)