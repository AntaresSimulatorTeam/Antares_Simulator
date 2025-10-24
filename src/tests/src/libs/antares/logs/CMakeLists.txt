include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(Logs
        SRC
        logs-test.cpp
        logs.cpp
        ${CMAKE_SOURCE_DIR}/libs/antares/logs/logs-functions.cpp
        LIBS
        yuni-static-core
)

target_include_directories(Logs
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/src/libs/antares/logs/include")
