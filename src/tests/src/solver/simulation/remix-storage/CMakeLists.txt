include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(tests-on-hydro-remix
        SRC
        test-hydro-remix.cpp
        LIBS
        shave-peaks-by-remix-storage-gen
        test_utils_unit)