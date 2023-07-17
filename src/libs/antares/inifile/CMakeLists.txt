set(SRC_INI
        inifile.h
        inifile.hxx
        inifile.cpp
        )

source_group("misc\\inifile" FILES ${SRC_INI})

add_library(inifile
        ${SRC_INI}
)

target_link_libraries(inifile
        PRIVATE
        io
        yuni-static-core
        )

target_include_directories(inifile
        PRIVATE
        ${CMAKE_SOURCE_DIR}/ext/yuni/src
        )