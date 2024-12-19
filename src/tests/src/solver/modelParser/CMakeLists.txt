# Add source files
set(SOURCE_FILES
        testModelParser.cpp
        testModelTranslator.cpp
        testConvertorVisitor.cpp
        test_full.cpp
        enum_operators.h
        testSystemParser.cpp
        testSystemConverter.cpp
)

# Add executable
add_executable(TestModelParser ${SOURCE_FILES})

# Link libraries
target_link_libraries(TestModelParser
        PRIVATE
        Boost::unit_test_framework
        Antares::solver-expressions
        Antares::modelConverter
        Antares::modelParser
        Antares::systemParser
        Antares::antares-study-system-model
        Antares::antlr-interface
)

# Storing test-toybox under the folder Unit-tests in the IDE
set_target_properties(${EXECUTABLE_NAME} PROPERTIES FOLDER Unit-tests)

# Add the test
add_test(NAME TestModelParser COMMAND TestModelParser)

# Set test properties
set_property(TEST TestModelParser PROPERTY LABELS unit)
