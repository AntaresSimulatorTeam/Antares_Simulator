find_package(Boost COMPONENTS unit_test_framework REQUIRED)
enable_testing()

#bigobj support needed for windows compilation
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif(MSVC)

add_executable(tests-simple-study
        simple-study.cpp
)

target_link_libraries(tests-simple-study
        PRIVATE
        antares-solver-hydro
        antares-solver-variable
        antares-solver-simulation
        antares-solver-ts-generator
        model_antares
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        Antares::tests::in-memory-study
)

target_include_directories(tests-simple-study
        PRIVATE
        ${CMAKE_SOURCE_DIR}/solver
)

add_test(NAME end-to-end-simple-study COMMAND tests-simple-study)
set_property(TEST end-to-end-simple-study PROPERTY LABELS end-to-end)
set_target_properties(tests-simple-study PROPERTIES FOLDER Unit-tests/end_to_end)

# Storing tests-simple-study under the folder Unit-tests in the IDE

#----------------------------------------------------------
