set(src_tests_src_libs_antares_study "${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(area)
add_subdirectory(scenario-builder)
add_subdirectory(output-folder)
add_subdirectory(short-term-storage-input)
add_subdirectory(thermal-price-definition)
add_subdirectory(constraint)
add_subdirectory(series)

add_executable(test-study)
target_sources(test-study PRIVATE test_study.cpp)
target_link_libraries(test-study
        PRIVATE
        Antares::study
        Boost::unit_test_framework
)

add_test(NAME test-study COMMAND test-study)
