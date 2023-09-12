set(SRC_CHECKS
        checkLoadedInputData.cpp
        antares/checks/checkLoadedInputData.h
)
source_group("checks" FILES ${SRC_CHECKS})

add_library(checks
        ${SRC_CHECKS}
)
add_library(Antares::checks ALIAS checks)

target_include_directories(checks
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(checks
        PRIVATE
        logs
        yuni-static-core
        exception
        Antares::study # cpp -> area
        libantares-core
)