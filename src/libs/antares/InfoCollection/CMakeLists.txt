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
)

target_include_directories(${PROJ}
        PUBLIC
        ${CMAKE_SOURCE_DIR}/libs #study
        ${CMAKE_SOURCE_DIR}/libs/antares/date #parameters -> date
        ${CMAKE_SOURCE_DIR}/libs/antares/array #links
        ${CMAKE_SOURCE_DIR}/libs/antares/logs #study -> binding constraint -> matrix -> jit
        ${CMAKE_SOURCE_DIR}/libs/antares/io #study -> binding constraint -> matrix
        ${CMAKE_SOURCE_DIR}/libs/antares/writer #study
        ${CMAKE_SOURCE_DIR}/libs/antares/exception #enum -> exception
        ${CMAKE_SOURCE_DIR}/libs/antares/jit # something,something -> jit
        ${CMAKE_SOURCE_DIR}/libs/antares/memory/include #study -> BC -> link -> memory
        ${CMAKE_SOURCE_DIR}/libs/antares/inifile #study -> inifile
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
