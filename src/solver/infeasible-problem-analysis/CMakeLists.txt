project(infeasible-problem-analysis)

set(SRC_INFEASIBLE_PROBLEM_ANALYSIS
  unfeability-diagnostic.h 
  unfeasible-pb-analysis.cpp
  unfeasible-pb-analysis.h
  report.h
  report.cpp
  constraint.h
  constraint.cpp
  )

add_library(infeasible_problem_analysis ${SRC_INFEASIBLE_PROBLEM_ANALYSIS})
target_link_libraries(infeasible_problem_analysis
  PUBLIC ortools::ortools sirius_solver
        utils #ortools-utils, not Antares::utils
  )
