set(EXECUTABLE_NAME test-expressions)
add_executable(${EXECUTABLE_NAME})

target_sources(${EXECUTABLE_NAME}
        PRIVATE
        test_main.cpp
        test_nodes.cpp
        test_PrintAndEvalNodes.cpp
        test_TimeIndexVisitor.cpp
        test_SubstitutionVisitor.cpp
        test_LinearVisitor.cpp
        test_CompareVisitor.cpp
        test_CloneVisitor.cpp
	test_DeepWideTrees.cpp
)

target_link_libraries(${EXECUTABLE_NAME}
        PRIVATE
        Boost::unit_test_framework
        antares-solver-expressions
)

# Storing tests-ts-numbers under the folder Unit-tests in the IDE
set_target_properties(${EXECUTABLE_NAME} PROPERTIES FOLDER Unit-tests)

add_test(NAME test-expressions COMMAND ${EXECUTABLE_NAME})

set_property(TEST test-expressions PROPERTY LABELS unit)
