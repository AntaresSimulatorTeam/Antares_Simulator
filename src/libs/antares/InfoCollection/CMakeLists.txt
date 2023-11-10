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
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs> #study
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/antares/date> #parameters -> date
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/antares/array> #links
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/antares/logs> #study -> binding constraint -> matrix -> jit
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/antares/io> #study -> binding constraint -> matrix
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/antares/writer> #study
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/antares/exception> #enum -> exception
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/antares/jit> # something,something -> jit
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/antares/memory/include> #study -> BC -> link -> memory
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/antares/inifile> #study -> inifile
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
