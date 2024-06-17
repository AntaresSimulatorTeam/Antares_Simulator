set(PROJ file-tree-study-loader)
set(SRC_PROJ
        include/antares/file-tree-study-loader/FileTreeStudyLoader.h
        FileTreeStudyLoader.cpp
)
source_group("file-study-tree-loader" FILES ${SRC_PROJ})

add_library(${PROJ})
add_library(Antares::${PROJ} ALIAS ${PROJ})
target_sources(${PROJ}
        PRIVATE
        ${SRC_PROJ}
)

target_link_libraries(${PROJ}
        PUBLIC
        Antares::study-loader
        Antares::result_writer
        Antares::study
        Antares::application
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)