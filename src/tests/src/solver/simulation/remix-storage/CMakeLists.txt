include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

set(include_dir ${CMAKE_SOURCE_DIR}/solver/simulation/include)

#-----------------------------
# Unit tests on utils
#-----------------------------
add_boost_test(tests-on-remix-utils
        SRC
        tests-on-remix-utils.cpp
        INCLUDE
        ${include_dir}
        LIBS
        storage-for-remix-list-sort
        Antares::vector-utils
        test_utils_unit)

set_target_properties(tests-on-remix-utils PROPERTIES FOLDER Unit-tests/storage-remix)


#---------------------------------------
# Unit tests on remix algo components
#---------------------------------------
add_boost_test(tests-on-shave-peak-by-remix-help
        SRC
        tests-on-shave-peak-by-remix-help.cpp
        LIBS
        shave-peaks-by-remix-storage-gen
        test_utils_unit)

set_target_properties(tests-on-shave-peak-by-remix-help PROPERTIES FOLDER Unit-tests/storage-remix)

#-----------------------------
# Unit tests on hydro remix
#-----------------------------
add_boost_test(tests-on-hydro-remix
        SRC
        test-hydro-remix.cpp
        LIBS
        shave-peaks-by-remix-storage-gen
        Antares::vector-utils
        test_utils_unit)

set_target_properties(tests-on-hydro-remix PROPERTIES FOLDER Unit-tests/storage-remix)


#-----------------------------
# Unit tests on hydro remix
#-----------------------------
add_boost_test(tests-on-multi-storage-remix
        SRC
        tests-multi-storage-remix.cpp
        tests-collect_remix_debug_info.cpp
        LIBS
        shave-peaks-by-remix-storage-gen
        Antares::vector-utils
        test_utils_unit)

set_target_properties(tests-on-multi-storage-remix PROPERTIES FOLDER Unit-tests/storage-remix)

#-----------------------------
# Unit tests on extractSTS-functions
#-----------------------------
add_boost_test(tests-on-extractSTS-functions
        SRC
        tests-extractSTS-functions.cpp
        INCLUDE
        ${include_dir}
        LIBS
        antares-solver-simulation
        test_utils_unit)

set_target_properties(tests-on-extractSTS-functions PROPERTIES FOLDER Unit-tests/storage-remix)