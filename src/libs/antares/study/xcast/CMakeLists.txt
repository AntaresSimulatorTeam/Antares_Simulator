set(SRC_XCAST
        xcast.h
        xcast.hxx
        xcast.cpp
        )
source_group("study\\xcast" FILES ${SRC_XCAST})

add_library(xcast
        ${SRC_XCAST}
        )

target_include_directories(xcast
        PRIVATE
        ${CMAKE_SOURCE_DIR}/ext/yuni/src
        ${CMAKE_SOURCE_DIR}/libs
        ${CMAKE_SOURCE_DIR}/solver/writer
        )

target_link_libraries(xcast
        PRIVATE
        yuni-static-core
        )