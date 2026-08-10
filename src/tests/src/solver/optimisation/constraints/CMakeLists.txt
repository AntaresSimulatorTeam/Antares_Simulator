include(${CMAKE_SOURCE_DIR}/src/tests/macros.cmake)

add_boost_test(constraints_builder
        SRC
        constraints_builder.cpp
        LIBS
	solver-lib)

add_boost_test(constraints_namer
        SRC
        constraints_namer.cpp
        LIBS
	model_antares)

add_boost_test(legacy_variable_info_namer
        SRC
        legacy_variable_info_namer.cpp
        LIBS
	model_antares)

