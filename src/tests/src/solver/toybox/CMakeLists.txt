# Define the executable name
set(EXECUTABLE_NAME test-toybox)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(antlr4-runtime CONFIG REQUIRED)

# Add the executable
add_executable(${EXECUTABLE_NAME} testToybox.cpp)

# Include directories
target_include_directories(${EXECUTABLE_NAME}
        PRIVATE
        "${src_solver_optimisation}"
)

# Link libraries
target_link_libraries(${EXECUTABLE_NAME}
        PRIVATE
        Boost::unit_test_framework
        Antares::modeler-ortools-impl
        yaml-cpp
)

# Storing test-toybox under the folder Unit-tests in the IDE
set_target_properties(${EXECUTABLE_NAME} PROPERTIES FOLDER Unit-tests)

# Add the test
add_test(NAME test-toybox COMMAND ${EXECUTABLE_NAME})

# Set test properties
set_property(TEST test-toybox PROPERTY LABELS unit)