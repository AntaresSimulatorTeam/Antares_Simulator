include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(tests-on-hydro-remix
        SRC
        test-hydro-remix.cpp
        LIBS
        shave-peaks-by-remix-storage-gen
        test_utils_unit)

set_target_properties(tests-on-hydro-remix PROPERTIES FOLDER Unit-tests/storage-remix)


add_boost_test(tests-on-remix-utils
        SRC
        tests-on-remix-utils.cpp
        LIBS
        shave-peaks-by-remix-storage-gen
        test_utils_unit)
		
set_target_properties(tests-on-remix-utils PROPERTIES FOLDER Unit-tests/storage-remix)


add_boost_test(tests-on-shave-peak-by-remix-help
        SRC
        tests-on-shave-peak-by-remix-help.cpp
        LIBS
        shave-peaks-by-remix-storage-gen
        test_utils_unit)
		
set_target_properties(tests-on-shave-peak-by-remix-help PROPERTIES FOLDER Unit-tests/storage-remix)