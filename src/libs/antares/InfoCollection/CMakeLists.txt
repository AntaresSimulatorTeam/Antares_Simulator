set(PROJ infoCollection)
set(HEADERS
        include/antares/${PROJ}/StudyInfoCollector.h
)
set(SRC_PROJ
        ${HEADERS}
        StudyInfoCollector.cpp
)
source_group("misc\\${PROJ}" FILES ${SRC_PROJ})

add_library(${PROJ} ${SRC_PROJ})
add_library(Antares::${PROJ} ALIAS ${PROJ})

target_link_libraries(${PROJ}
        PRIVATE
        yuni-static-core
        PUBLIC
        Antares::benchmarking
        Antares::study
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)