set(SRC_OBJECT
        intrusive-reference.h
        intrusive-reference.hxx
        object.h
        object.hxx
        object.cpp
        ref.h
        )
source_group("object" FILES ${SRC_OBJECT})

add_library(object
        ${SRC_OBJECT})

target_include_directories(object
        PRIVATE
        ${CMAKE_SOURCE_DIR}/ext/yuni/src
        )

target_link_libraries(object
        PRIVATE
        yuni-static-core
        yuni-static-uuid
        )