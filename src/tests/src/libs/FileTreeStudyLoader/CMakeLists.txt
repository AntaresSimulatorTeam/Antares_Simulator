set(EXECUTABLE_NAME test-file-study-loader)
add_executable(${EXECUTABLE_NAME})

target_sources(${EXECUTABLE_NAME}
        PRIVATE
        test_file-study-loader.cpp
)

target_link_libraries(${EXECUTABLE_NAME}
        PRIVATE
        Boost::unit_test_framework
        Antares::study-loader
        Antares::file-tree-study-loader
)

# Storing tests-ts-numbers under the folder Unit-tests in the IDE
set_target_properties(${EXECUTABLE_NAME} PROPERTIES FOLDER Unit-tests)

add_test(NAME test-file-study-loader COMMAND ${EXECUTABLE_NAME})

set_property(TEST test-file-study-loader PROPERTY LABELS unit)
