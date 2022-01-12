# Useful variables definitions
set(CMAKE_CXX_STANDARD 17)
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")

# ====================================
# Tests on reading scenario-builder
# ====================================
set(SRC_SC_BUILDER_READ
	test-sc-builder-file-read-line.cpp
)
add_executable(test-sc-builder-file-read-line ${SRC_SC_BUILDER_READ})

target_include_directories(test-sc-builder-file-read-line
								PRIVATE
									"${src_libs_antares_study}"
									"${src_libs_antares_study}/scenario-builder"
)

target_link_libraries(test-sc-builder-file-read-line
						PRIVATE
							Boost::unit_test_framework
							libantares-core
							libmodel_antares
)
# Linux
if(UNIX AND NOT APPLE)
target_link_libraries(test-sc-builder-file-read-line PRIVATE stdc++fs)
endif()


# Storing test-sc-builder-file-read-line under the folder Unit-tests in the IDE
set_target_properties(test-sc-builder-file-read-line PROPERTIES FOLDER Unit-tests/sc-builder)

add_test(NAME sc-builder-file-read-line COMMAND test-sc-builder-file-read-line)

set_property(TEST sc-builder-file-read-line PROPERTY LABELS TESTLABEL unit)

# ====================================
# Tests on saving scenario-builder
# ====================================
set(SRC_SC_BUILDER_SAVE
	test-sc-builder-file-save.cpp
	"${src_tests_src_libs_antares_study}/area/files-helper.cpp"
)
add_executable(test-sc-builder-file-save ${SRC_SC_BUILDER_SAVE})

target_include_directories(test-sc-builder-file-save
								PRIVATE
									"${src_libs_antares_study}"
									"${src_libs_antares_study}/scenario-builder"
									"${src_tests_src_libs_antares_study}"
)

target_link_libraries(test-sc-builder-file-save
  PRIVATE
						Boost::unit_test_framework
						libantares-core
						libmodel_antares
)

# Storing test-sc-builder-file-savee under the folder Unit-tests in the IDE
set_target_properties(test-sc-builder-file-save PROPERTIES FOLDER Unit-tests/sc-builder)

add_test(NAME sc-builder-file-save COMMAND test-sc-builder-file-save)

set_property(TEST sc-builder-file-save PROPERTY LABELS TESTLABEL unit)
