include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(unit-tests-for-modeler-api
        SRC
        test_main.cpp
        testLinearProblemBuilder.cpp
        testLinearProblemMpsolverImpl.cpp
        mock-fillers/FillerContext.h
        mock-fillers/OneConstraintFiller.h
        mock-fillers/OneVarFiller.h
        mock-fillers/TwoVarsTwoConstraintsFiller.h
        LIBS
        Antares::linear-problem-mpsolver-impl
        Antares::linear-problem-data-impl)

