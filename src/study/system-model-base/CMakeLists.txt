project(system-model-base)

# This is a header-only (interface) library: declare it as INTERFACE so
# CMake does not try to determine a linker language from headers.
add_library(system-model-base INTERFACE)

add_library(Antares::system-model-base ALIAS system-model-base)

target_include_directories(system-model-base
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(system-model-base INTERFACE
        include/antares/study/system-model-base/variabilityType.h
)

install(DIRECTORY include/antares
        DESTINATION "include"
)