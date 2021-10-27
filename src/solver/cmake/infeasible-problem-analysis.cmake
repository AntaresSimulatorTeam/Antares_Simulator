set(SRC_INFEASIBLE_PROBLEM_ANALYSIS
  infeasible-problem-analysis/problem.cpp
  infeasible-problem-analysis/problem.h
  infeasible-problem-analysis/report.h
  infeasible-problem-analysis/report.cpp
  )

add_library(infeasible_problem_analysis ${SRC_INFEASIBLE_PROBLEM_ANALYSIS})
target_link_libraries(infeasible_problem_analysis
  PUBLIC ortools::ortools sirius_solver
  PRIVATE libantares-core # antares/study/OrtoolsSolver.cpp
  )
