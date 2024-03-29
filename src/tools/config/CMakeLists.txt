
OMESSAGE("antares-config")


# Le main
set(SRCS main.cpp)


if (WIN32)
    FILE(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/win32/config.o")
    CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/win32/config.rc.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/win32/config.rc")
    FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/win32/config.ico" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/win32/")
    SET(SRCS ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/win32/config.rc")
endif (WIN32)


set(execname "antares-config")
add_executable(${execname} ${SRCS})
install(TARGETS ${execname} EXPORT antares-config DESTINATION bin)

INSTALL(EXPORT antares-config
        FILE antares-configConfig.cmake
        DESTINATION cmake
)

set(CONFIG_LIBS
        yuni-static-core
        Antares::args_helper
        ${CMAKE_THREADS_LIBS_INIT}
)

# The new ant library
target_include_directories(${execname}
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/libs"
)

target_link_libraries(${execname}
        PRIVATE
        ${CONFIG_LIBS}
        Antares::array
        Antares::sys
        Antares::resources
        Antares::locale
        antares-core #ressource.h
)

import_std_libs(${execname})
executable_strip(${execname})

