project(AntaresAnalyzer)

cmake_minimum_required(VERSION 3.0)

include(../cmake/messages.cmake)
OMESSAGE("antares-analyzer")

include(../cmake/common-settings.cmake)


# Le main
Set(SRCS main.cpp
        atsp/atsp.h
        atsp/atsp.hxx
        atsp/atsp.cpp
        atsp/load.cpp
        atsp/misc.cpp
        atsp/preflight.cpp
        atsp/correlations.cpp
        atsp/cache.cpp
)


if (WIN32)
    file(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/win32/analyzer.o")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/win32/analyzer.rc.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/win32/analyzer.rc")

    if (MINGW)
        # resource compilation for mingw
        ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/win32/analyzer.o"
                COMMAND windres.exe "-I${CMAKE_CURRENT_SOURCE_DIR}"
                "-i${CMAKE_CURRENT_SOURCE_DIR}/win32/analyzer.rc"
                -o "${CMAKE_CURRENT_BINARY_DIR}/win32/analyzer.o")
        SET(SRCS ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/win32/analyzer.o")
    else (MINGW)
        FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/win32/analyzer.ico" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/win32/")
        SET(SRCS ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/win32/analyzer.rc")
    endif (MINGW)
endif (WIN32)

add_executable(antares-analyzer  ${SRCS})

install(TARGETS antares-analyzer EXPORT antares-analyzer DESTINATION bin)

INSTALL(EXPORT antares-analyzer
        FILE antares-analyzerConfig.cmake
        DESTINATION cmake
)

target_link_libraries(antares-analyzer
        PRIVATE
        yuni-static-core
        Antares::args_helper
        ${CMAKE_THREADS_LIBS_INIT}
        antares-core #local.h
        Antares::misc
        Antares::locale
        PUBLIC
        Antares::study #atsp -> xcast
        Antares::sys
)

import_std_libs(antares-analyzer)
executable_strip(antares-analyzer)

