# Useful variables definitions
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")
set(src_libs_antares "${CMAKE_SOURCE_DIR}/libs/antares")

add_executable(test-hydro-data-transfer test-datatransfer-class.cpp)

target_include_directories(test-hydro-data-transfer
   						   PRIVATE
							"${src_libs_antares_study}"
						    "${src_libs_antares_study}/parts/hydro"
						    "${src_libs_antares}/array"
)

target_link_libraries(test-hydro-data-transfer
    				  PRIVATE
					  Boost::unit_test_framework
					  libantares-core
)

# Linux
if(UNIX AND NOT APPLE)
target_link_libraries(test-hydro-data-transfer PRIVATE stdc++fs)
endif()

set_target_properties(test-hydro-data-transfer PROPERTIES FOLDER Unit-tests)
add_test(NAME test-hydro-data-transfer COMMAND test-hydro-data-transfer)
set_property(TEST test-hydro-data-transfer PROPERTY LABELS unit)