# Useful variables definitions
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")

add_executable(test-folder-output study.cpp)

target_include_directories(test-folder-output
   						   PRIVATE
						   "${src_libs_antares_study}"
)

target_link_libraries(test-folder-output
    				  PRIVATE
					  Boost::unit_test_framework
					  libantares-core
						Antares::study
)

# Linux
if(UNIX AND NOT APPLE)
target_link_libraries(test-folder-output PRIVATE stdc++fs)
endif()


# Storing test-save-link-properties under the folder Unit-tests in the IDE
set_target_properties(test-folder-output PROPERTIES FOLDER Unit-tests)

import_std_libs(test-folder-output)

add_test(NAME folder-output COMMAND test-folder-output)
set_property(TEST folder-output PROPERTY LABELS unit)


