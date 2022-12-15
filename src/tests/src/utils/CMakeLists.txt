add_library(test_utils_unit
        utils.cpp
        utils.h
        )

target_include_directories(
        test_utils_unit
                PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )

set_target_properties(test_utils_unit PROPERTIES FOLDER Unit-tests)

