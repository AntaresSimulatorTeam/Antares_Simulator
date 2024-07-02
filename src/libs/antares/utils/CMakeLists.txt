set(PROJ StudyUtils)
set(SRC_PROJ
        utils.cpp
        include/antares/utils/utils.h
        include/antares/utils/utils.hxx
)
source_group("utils" FILES ${SRC_PROJ})

add_library(${PROJ} ${SRC_PROJ}) #utils also used elsewhere
add_library(Antares::utils ALIAS ${PROJ})

target_link_libraries(${PROJ}
        PRIVATE
        yuni-static-core
        Antares::logs
)

target_include_directories(${PROJ}
                PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
