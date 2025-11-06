include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(Enums
        SRC
        Enum-test.cpp
        LIBS
        Antares::stdcxx
        Antares::exception
)

target_include_directories(Enums
        PRIVATE
        "${CMAKE_SOURCE_DIR}/libs/antares/include")
