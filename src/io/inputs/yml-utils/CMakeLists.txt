set(SOURCES
        path_utils.cpp
        include/antares/io/inputs/yml-utils/path_utils.h
        include/antares/io/inputs/yml-utils/YmlMapMarker.h
)

add_library(yml-utils ${SOURCES})
add_library(Antares::yml-utils ALIAS yml-utils)

target_include_directories(yml-utils
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(yml-utils
        PUBLIC
        fmt::fmt
        PRIVATE
        yaml-cpp::yaml-cpp
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
