set(SRC_INI
        inifile/inifile.h
        inifile/inifile.hxx
        inifile.cpp
        )

source_group("misc\\inifile" FILES ${SRC_INI})

add_library(inifile
        ${SRC_INI}
)

target_link_libraries(inifile
        PRIVATE
        io
        logs
        yuni-static-core
        )

target_include_directories(inifile
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )