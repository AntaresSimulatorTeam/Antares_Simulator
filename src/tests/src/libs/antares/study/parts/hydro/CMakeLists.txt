# Useful variables definitions
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")
set(src_libs_antares "${CMAKE_SOURCE_DIR}/libs/antares")

# Hydro data transfer
set(SRC_HYDRO_READER
	test-hydroreader-class.cpp
	help-functions.h
	help-functions.cpp)

add_executable(test-hydro-reader ${SRC_HYDRO_READER})

target_include_directories(test-hydro-reader
	PRIVATE
	"${src_libs_antares_study}"
	"${src_libs_antares_study}/parts/hydro"
	"${src_libs_antares}/array/antares/array"
)

target_link_libraries(test-hydro-reader
	PRIVATE
	Boost::unit_test_framework
	antares-core
	model_antares
	antares-solver-hydro
	yuni-static-core
)

# Linux
if(UNIX AND NOT APPLE)
	target_link_libraries(test-hydro-reader PRIVATE stdc++fs)
endif()

set_target_properties(test-hydro-reader PROPERTIES FOLDER Unit-tests)
add_test(NAME test-hydro-reader COMMAND test-hydro-reader)
set_property(TEST test-hydro-reader PROPERTY LABELS unit)

# Useful variables definitions
set(src_libs_antares_study "${CMAKE_SOURCE_DIR}/libs/antares/study")
set(src_libs_antares "${CMAKE_SOURCE_DIR}/libs/antares")

# Hydro series
set(SRC_HYDRO_SERIES
	test-hydro-series.cpp
	help-functions.h
	help-functions.cpp)

add_executable(test-hydro-series ${SRC_HYDRO_SERIES})

target_include_directories(test-hydro-series
	PRIVATE
	"${src_libs_antares_study}"
	"${src_libs_antares_study}/parts/hydro"
	"${src_libs_antares}/array/antares/array"
	"${src_libs_antares}/array/antares/array"
	"${src_libs_antares}/exception/antares/exception"
)

target_link_libraries(test-hydro-series
	PRIVATE
	Boost::unit_test_framework
	antares-core
	model_antares
	antares-solver-hydro
	yuni-static-core
	exception
)

# Linux
if(UNIX AND NOT APPLE)
	target_link_libraries(test-hydro-series PRIVATE stdc++fs)
endif()

set_target_properties(test-hydro-series PROPERTIES FOLDER Unit-tests)
add_test(NAME test-hydro-series COMMAND test-hydro-series)
set_property(TEST test-hydro-series PROPERTY LABELS unit)