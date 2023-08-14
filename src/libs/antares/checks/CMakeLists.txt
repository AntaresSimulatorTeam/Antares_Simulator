set(SRC_CHECKS
        checkLoadedInputData.cpp
        antares/checks/checkLoadedInputData.h
)
source_group("checks" FILES ${SRC_CHECKS})

add_library(checks
        ${SRC_CHECKS}
)

target_include_directories(checks
        PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/
        ${CMAKE_SOURCE_DIR}/libs/antares/study
        ${CMAKE_SOURCE_DIR}/libs/antares/inifile #adq-patch-params
        ${CMAKE_SOURCE_DIR}/libs/antares/array #area
        ${CMAKE_SOURCE_DIR}/libs/antares/io #area -> matrix -> io
        ${CMAKE_SOURCE_DIR}/libs/antares/date #area -> parameter -> date
        ${CMAKE_SOURCE_DIR}/libs/antares/writer #area -> antares/parts/thermal/cluster -> writer
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(checks
        PRIVATE
        logs
        yuni-static-core
)