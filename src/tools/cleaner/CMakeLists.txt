
OMESSAGE("antares-study-cleaner")


# Le main
Set(SRCS main.cpp
)

if (WIN32 OR WIN64)
    FILE(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/win32/cleaner.o")
    CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/win32/cleaner.rc.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/win32/cleaner.rc")
    FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/win32/cleaner.ico" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/win32/")
    SET(SRCS ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/win32/cleaner.rc")
endif ()


set(execname "antares-study-cleaner")
add_executable(${execname}  ${SRCS})
install(TARGETS ${execname} EXPORT antares-study-cleaner DESTINATION bin)

INSTALL(EXPORT antares-study-cleaner
        FILE antares-study-cleanerConfig.cmake
        DESTINATION cmake
)

set(CLEANER_LIBS
        yuni-static-core
        Antares::args_helper
        Antares::study
        Antares::sys
        antares-core #version.h
        Antares::locale
        ${CMAKE_THREADS_LIBS_INIT}
)

# The new ant library
target_include_directories(${execname}
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/libs"
)

target_link_libraries(${execname}
        PRIVATE
        ${CLEANER_LIBS}
)

import_std_libs(${execname})
executable_strip(${execname})

