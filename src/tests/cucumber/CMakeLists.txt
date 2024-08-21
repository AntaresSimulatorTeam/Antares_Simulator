include_guard(GLOBAL)

find_program(Cucumber NAMES cucumber)

if (Cucumber)
    #find_package(Boost COMPONENTS unit_test_framework system program_options regex filesystem REQUIRED)
    find_package(GTest REQUIRED)
    find_package(CucumberCpp COMPONENTS cucumber-cpp REQUIRED)

    add_executable(AntaresSteps features/step_definitions/AntaresSteps.cpp)
    #target_link_libraries(AntaresSteps PRIVATE Boost::unit_test_framework CucumberCpp::cucumber-cpp solver_api)
    target_link_libraries(AntaresSteps PRIVATE GTest::gtest CucumberCpp::cucumber-cpp solver_api)
endif()