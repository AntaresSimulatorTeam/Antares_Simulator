project(AntaresStudyYearByYearAggregator)
cmake_minimum_required(VERSION 2.8)

include(../../cmake/messages.cmake)
OMESSAGE("antares-ybyaggregator")


include(../../cmake/common-settings.cmake)


# Le main
set(SRCS
        main.cpp
        datafile.h
        result.h
        result.cpp
        output.h
        output.cpp
        job.h
        job.hxx
        job.cpp
        progress.h
        progress.hxx
        progress.cpp
)

if (WIN32 OR WIN64)
    FILE(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/win32/ybyaggregator.o")
    CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/win32/ybyaggregator.rc.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/win32/ybyaggregator.rc")
    FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/win32/ybyaggregator.ico" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/win32/")
    SET(SRCS ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/win32/ybyaggregator.rc")
endif ()


set(execname "antares-ybyaggregator")
add_executable(${execname}  ${SRCS})
install(TARGETS ${execname} EXPORT antares-ybyaggregator DESTINATION bin)

INSTALL(EXPORT antares-ybyaggregator
        FILE antares-ybyaggregatorConfig.cmake
        DESTINATION cmake
)

set(YBY_AGGREGATOR_LIBS
        antares-core #version.h
        Antares::args_helper
        Antares::date
        Antares::logs
        yuni-static-core
        Antares::sys
        Antares::locale
        ${wxWidgets_LIBRARIES} ${CMAKE_THREADS_LIBS_INIT})

# The new ant library
target_include_directories(${execname}
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/libs"
)

target_link_libraries(${execname}
        PRIVATE
        ${YBY_AGGREGATOR_LIBS}
        antares-solver-ts-generator
        Antares::memory
        Antares::utils
)

import_std_libs(${execname})
executable_strip(${execname})

