# Useful variables definitions
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")

# ====================================
# Tests on reading scenario-builder
# ====================================
set(SRC_SC_BUILDER_READ
	short-term-storage-input.cpp
)
add_executable(short-term-storage-input ${SRC_SC_BUILDER_READ})

target_include_directories(short-term-storage-input
								PRIVATE
									"${src_libs_antares_study}/parts/short-term-storage"
)

target_link_libraries(short-term-storage-input
						PRIVATE
							Boost::unit_test_framework
							libmodel_antares
)
# Linux
if(UNIX AND NOT APPLE)
target_link_libraries(short-term-storage-input PRIVATE stdc++fs)
endif()


# Storing short-term-storage-input under the folder Unit-tests in the IDE
set_target_properties(short-term-storage-input PROPERTIES FOLDER Unit-tests/short-term-storage-input)

add_test(NAME short-term-storage-input COMMAND short-term-storage-input)

set_property(TEST short-term-storage-input PROPERTY LABELS unit)
