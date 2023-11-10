set(PROJ signal-handling)
set(HEADERS
        include/antares/${PROJ}/common.h
        include/antares/${PROJ}/public.h
)
set(SRC_SIGNAL
        ${HEADERS}
        common.cpp
        linux.cpp
        windows.cpp
)
source_group("misc\\${PROJ}" FILES ${SRC_SIGNAL})

add_library(${PROJ} ${SRC_SIGNAL})
add_library(Antares::${PROJ} ALIAS ${PROJ})

target_link_libraries(${PROJ}
        PRIVATE
        yuni-static-core
        Antares::study
        PUBLIC
        Antares::result_writer
        Antares::memory
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
