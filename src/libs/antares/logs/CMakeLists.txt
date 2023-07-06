set(SRC_LOGS
        logs.h
        logs.hxx
        logs.cpp
        cleaner.h
        cleaner.cpp
        )
source_group("misc\\logs" FILES ${SRC_LOGS})

add_library(logs ${SRC_LOGS})

target_link_libraries(logs
        PRIVATE
        yuni-static-core
        )

target_include_directories(logs
        PRIVATE
        ${CMAKE_SOURCE_DIR}/ext/yuni/src
        )