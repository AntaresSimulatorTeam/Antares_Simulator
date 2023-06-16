# Useful variables definitions
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")

# ====================================
# Tests on thermal price definition
# ====================================
set(SRC_THERM_PRICE_DEF
	thermal-price-definition.cpp
)
add_executable(thermal-price-definition ${SRC_THERM_PRICE_DEF})

target_include_directories(thermal-price-definition
								PRIVATE
									"${src_libs_antares_study}/parts/thermal"
									"${src_libs_antares_study}/area"
									"${src_libs_antares_study}"
)

target_link_libraries(thermal-price-definition
						PRIVATE
							Boost::unit_test_framework
							libantares-core
							libmodel_antares
)
# Linux
if(UNIX AND NOT APPLE)
target_link_libraries(thermal-price-definition PRIVATE stdc++fs)
endif()


# Storing thermal-price-definition under the folder Unit-tests in the IDE
set_target_properties(thermal-price-definition PROPERTIES FOLDER Unit-tests/thermal-price-definition)

add_test(NAME thermal-price-definition COMMAND thermal-price-definition)

set_property(TEST thermal-price-definition PROPERTY LABELS unit)
