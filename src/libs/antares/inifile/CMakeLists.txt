set(SRC_INI
        include/antares/inifile/inifile.h
        include/antares/inifile/inifile.hxx
        inifile.cpp
)

source_group("misc\\inifile" FILES ${SRC_INI})

add_library(inifile
        ${SRC_INI}
)
add_library(Antares::inifile ALIAS inifile)

target_link_libraries(inifile
        PUBLIC
        yuni-static-core
        PRIVATE
        io
        logs
        Boost::headers
)

target_include_directories(inifile
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)