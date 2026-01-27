set(PROJ test_structured_linear_problem)

set(SRC_TEST
        test_structured_linear_problem.cpp
        )

add_executable(${PROJ} ${SRC_TEST})
target_link_libraries(${PROJ})
        PRIVATE
        Boost::unit_test_framework
        linear-problem-api
)

add_test(NAME ${PROJ} COMMAND ${PROJ})
