set(EXECUTABLE_NAME test-translator)
add_executable(${EXECUTABLE_NAME})

target_sources(${EXECUTABLE_NAME}
        PRIVATE
        test_translator.cpp
)

target_link_libraries(${EXECUTABLE_NAME}
        PRIVATE
        Boost::unit_test_framework
        model_antares
)

# Storing tests-ts-numbers under the folder Unit-tests in the IDE
set_target_properties(${EXECUTABLE_NAME} PROPERTIES FOLDER Unit-tests)

add_test(NAME test-translator COMMAND ${EXECUTABLE_NAME})

set_property(TEST test-translator PROPERTY LABELS unit)
