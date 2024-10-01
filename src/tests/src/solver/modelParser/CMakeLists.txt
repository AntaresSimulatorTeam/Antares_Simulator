# Add source files
set(SOURCE_FILES
        testModelParser.cpp
        testModelTranslator.cpp
        test_full.cpp
)

# Add executable
add_executable(TestModelParser ${SOURCE_FILES})

# Link libraries
target_link_libraries(TestModelParser
        PRIVATE
        Boost::unit_test_framework
        Antares::modelConverter
        Antares::modelParser
        Antares::antares-solver-libObjectModel
)

# Storing test-toybox under the folder Unit-tests in the IDE
set_target_properties(${EXECUTABLE_NAME} PROPERTIES FOLDER Unit-tests)

# Add the test
add_test(NAME TestModelParser COMMAND TestModelParser)

# Set test properties
set_property(TEST TestModelParser PROPERTY LABELS unit)