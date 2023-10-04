set(SRC_ARGS_HELPER
        args_to_utf8.h
        args_to_utf8.cpp
)

source_group("misc\\args_helper" FILES ${SRC_ARGS_HELPER})

add_library(args_helper
        ${SRC_ARGS_HELPER}
)

add_library(Antares::args_helper ALIAS args_helper)

target_link_libraries(args_helper
        PRIVATE
			yuni-static-core
        )

target_include_directories(args_helper
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)