#Header only library, nothing is compiled hence usage of INTERFACE
add_library(study-loader INTERFACE)
add_library(Antares::study-loader ALIAS study-loader)

target_sources(study-loader INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/antares/study-loader/IStudyLoader.h>
)

target_include_directories(study-loader
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)