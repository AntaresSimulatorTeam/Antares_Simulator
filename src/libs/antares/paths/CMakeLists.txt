##########################
# Target : Paths list
##########################
set(SRC_PATHS_LIST
        list.cpp
        include/antares/paths/list.h
)
source_group("paths-list" FILES ${SRC_PATHS_LIST})

add_library(paths-list ${SRC_PATHS_LIST})
add_library(Antares::paths-list ALIAS paths-list)

target_link_libraries(paths-list
        PRIVATE
        yuni-static-core
        Antares::logs
)

target_include_directories(paths-list
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)

###############################
# Target : paths-normalize
###############################

set(SRC_PATHS_NORM
        normalize_paths.cpp
        include/antares/paths/normalize_paths.h
)
source_group("paths-normalize" FILES ${SRC_PATHS_NORM})

add_library(paths-normalize ${SRC_PATHS_NORM})
add_library(Antares::paths-normalize ALIAS paths-normalize)

target_link_libraries(paths-normalize)

target_include_directories(paths-normalize
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
