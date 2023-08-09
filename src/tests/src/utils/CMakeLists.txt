add_library(test_utils_unit
        utils.cpp
        utils.h
        )

target_include_directories(
        test_utils_unit
        PUBLIC
        .
        )

set_target_properties(test_utils_unit PROPERTIES FOLDER Unit-tests)

