project(infeasible-problem-analysis)

set(SRC_INFEASIBLE_PROBLEM_ANALYSIS
  problem.cpp
  problem.h
  report.h
  report.cpp
  constraint.h
  constraint.cpp
  exceptions.h
  exceptions.cpp
  )

add_library(infeasible_problem_analysis ${SRC_INFEASIBLE_PROBLEM_ANALYSIS})
target_link_libraries(infeasible_problem_analysis
  PUBLIC ortools::ortools sirius_solver
  PRIVATE libantares-core
        utils
  )
