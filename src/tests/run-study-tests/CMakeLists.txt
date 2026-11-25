set(STUDY_TEST_CONFIG "${CMAKE_BUILD_TYPE}")
if(CMAKE_CONFIGURATION_TYPES AND NOT STUDY_TEST_CONFIG)
    list(GET CMAKE_CONFIGURATION_TYPES 0 STUDY_TEST_CONFIG)
endif()

file(GENERATE OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/executables.ini" CONTENT
    "[executables]
antares-solver = $<TARGET_FILE:antares-solver>
antares-problem-generator = $<TARGET_FILE:antares-problem-generator>
"
    CONDITION $<STREQUAL:$<CONFIG>,${STUDY_TEST_CONFIG}>)

find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
    find_python_module(pytest)
    find_python_module(numpy)

    if(PYTHON_MODULE_pytest_FOUND AND PYTHON_MODULE_numpy_FOUND)
        add_test(
            NAME json
            COMMAND Python3::Interpreter -m pytest test_from_json.py -m json
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        )
        add_test(
            NAME unfeasible
            COMMAND Python3::Interpreter -m pytest test_unfeasible_problem.py -m unfeasible
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        )
        set_property(TEST unfeasible PROPERTY LABELS study sirius)
    else()
        message(FATAL_ERROR "Module pytest or numpy not installed : can't run python scripts for end to end tests")
    endif()
endif()
