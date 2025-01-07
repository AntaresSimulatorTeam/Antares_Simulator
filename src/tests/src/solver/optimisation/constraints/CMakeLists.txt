set(EXECUTABLE_NAME constraints_builder)
add_executable(${EXECUTABLE_NAME})

target_sources(${EXECUTABLE_NAME}
        PRIVATE
        constraints_builder.cpp
)

target_link_libraries(${EXECUTABLE_NAME}
        PRIVATE
        Boost::unit_test_framework
        model_antares
        antares-solver-simulation
)

# Storing tests-ts-numbers under the folder Unit-tests in the IDE
set_target_properties(${EXECUTABLE_NAME} PROPERTIES FOLDER Unit-tests)

add_test(NAME constraints_builder COMMAND ${EXECUTABLE_NAME})

set_property(TEST constraints_builder PROPERTY LABELS unit)
