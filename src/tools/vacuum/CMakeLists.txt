
OMESSAGE("antares-vacuum")

# Le main
set(SRCS
        main.cpp
        modified-inode.h
        modified-inode.cpp
        antares-study.cpp
        antares-study.h
        io.h
        io.cpp
)


if (WIN32)
    FILE(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/win32/vacuum.o")
    CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/win32/vacuum.rc.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/win32/vacuum.rc")
    FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/win32/vacuum.ico" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/win32/")
    SET(SRCS ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/win32/vacuum.rc")
endif (WIN32)


set(execname "antares-vacuum")
add_executable(${execname}  ${SRCS})
install(TARGETS ${execname} EXPORT antares-vacuum DESTINATION bin)

INSTALL(EXPORT antares-vacuum
        FILE antares-vacuumConfig.cmake
        DESTINATION cmake
)

set(VACUUM_LIBS
        Antares::fswalker
        yuni-static-core
        ${CMAKE_THREADS_LIBS_INIT}
        Antares::args_helper
        Antares::study
        Antares::locale
)

target_link_libraries(${execname}
        PRIVATE
        ${VACUUM_LIBS}
        antares-core #version.h
)

import_std_libs(${execname})
executable_strip(${execname})

