set(EXECUTABLE_NAME test-api)
add_executable(${EXECUTABLE_NAME})

target_sources(${EXECUTABLE_NAME}
    PRIVATE
    test_api.cpp
)

target_link_libraries(${EXECUTABLE_NAME}
        PRIVATE
        Boost::unit_test_framework
        Antares::solver_api
        Antares::tests::in-memory-study
	test_utils_unit
)

target_include_directories(${EXECUTABLE_NAME}
        PRIVATE
        #Allow to use the private headers
        $<TARGET_PROPERTY:Antares::solver_api,INCLUDE_DIRECTORIES>
)

# Storing tests-ts-numbers under the folder Unit-tests in the IDE
set_target_properties(${EXECUTABLE_NAME} PROPERTIES FOLDER Unit-tests)

add_test(NAME test-api COMMAND ${EXECUTABLE_NAME})

set_property(TEST test-api PROPERTY LABELS unit)
