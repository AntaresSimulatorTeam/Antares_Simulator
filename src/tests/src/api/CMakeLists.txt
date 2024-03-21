set(EXECUTABLE_NAME test-api)
add_executable(${EXECUTABLE_NAME})

target_sources(${EXECUTABLE_NAME}
    PRIVATE
    test_api.cpp
    ${CMAKE_SOURCE_DIR}/api/API.cpp
    ${CMAKE_SOURCE_DIR}/api/SimulationObserver.cpp
)

target_link_libraries(${EXECUTABLE_NAME}
        PRIVATE
        Boost::unit_test_framework
        Antares::study
        Antares::study-loader
        Antares::tests::in-memory-study
        Antares::run-mode
        Antares::lps
)

target_include_directories(${EXECUTABLE_NAME}
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/api/private>
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/api/include>
)

# Storing tests-ts-numbers under the folder Unit-tests in the IDE
set_target_properties(${EXECUTABLE_NAME} PROPERTIES FOLDER Unit-tests)

add_test(NAME test-api COMMAND ${EXECUTABLE_NAME})

set_property(TEST test-api PROPERTY LABELS unit)
