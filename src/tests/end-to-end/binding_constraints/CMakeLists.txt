find_package(Boost COMPONENTS unit_test_framework REQUIRED)
enable_testing()

#bigobj support needed for windows compilation
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif(MSVC)

add_executable(tests-binding_constraints
        test_binding_constraints.cpp
    )

target_link_libraries(tests-binding_constraints
        PRIVATE
        test_utils
        Boost::unit_test_framework
        model_antares
        antares-solver-simulation
        antares-solver-hydro
        antares-solver-ts-generator
    )

target_include_directories(tests-binding_constraints
        PRIVATE
        ${CMAKE_SOURCE_DIR}/solver
        ${CMAKE_CURRENT_SOURCE_DIR}/../utils
        )

add_test(NAME end-to-end-binding_constraints COMMAND tests-binding_constraints)
set_property(TEST end-to-end-binding_constraints PROPERTY LABELS end-to-end)
set_target_properties(tests-binding_constraints PROPERTIES FOLDER Unit-tests/end_to_end)
