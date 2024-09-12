set(FILES_TO_LINK
        mock-fillers/OneVarFiller.cpp
        mock-fillers/OneConstraintFiller.cpp
)
add_executable(test-modeler-LP-builder testModelerLPbuilder.cpp ${FILES_TO_LINK})

target_include_directories(test-modeler-LP-builder
        PRIVATE
        "${src_solver_optimisation}"
)

target_link_libraries(test-modeler-LP-builder
        PRIVATE
            Boost::unit_test_framework
            Antares::modeler-ortools-impl
)

set_target_properties(test-modeler-LP-builder PROPERTIES FOLDER Unit-tests)
add_test(NAME modeler-LP-builder COMMAND test-modeler-LP-builder)
set_property(TEST modeler-LP-builder PROPERTY LABELS unit)