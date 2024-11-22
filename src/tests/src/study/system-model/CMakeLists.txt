set(EXECUTABLE_NAME test-system-model)
add_executable(${EXECUTABLE_NAME})

target_sources(${EXECUTABLE_NAME}
        PRIVATE
        test_main.cpp
        test_component.cpp
        test_system.cpp
)

target_link_libraries(${EXECUTABLE_NAME}
        PRIVATE
        Boost::unit_test_framework
        antares-study-system-model
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES FOLDER Unit-tests)
add_test(NAME test-system-model COMMAND ${EXECUTABLE_NAME})
set_property(TEST test-system-model PROPERTY LABELS unit)
