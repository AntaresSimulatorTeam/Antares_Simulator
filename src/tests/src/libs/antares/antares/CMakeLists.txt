include(${CMAKE_SOURCE_DIR}/src/tests/macros.cmake)

add_boost_test(Enums
        SRC
        Enum-test.cpp
        LIBS
        Antares::exception
        Antares::enums
)

target_include_directories(Enums
        PRIVATE
        "${CMAKE_SOURCE_DIR}/src/libs/antares/include")
