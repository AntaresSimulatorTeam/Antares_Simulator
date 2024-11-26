add_library(test_utils_unit
        files-system.cpp
        files-system.h
	unit_test_utils.h
	unit_test_utils.cpp
        )

target_link_libraries(test_utils_unit
       PRIVATE
       Boost::unit_test_framework)

target_include_directories(
        test_utils_unit
                PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

set_target_properties(test_utils_unit PROPERTIES FOLDER Unit-tests)

