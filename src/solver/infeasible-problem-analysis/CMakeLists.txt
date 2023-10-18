project(infeasible-problem-analysis)

set(SRC_INFEASIBLE_PROBLEM_ANALYSIS
  unfeasibility-analysis.h
  unfeasibility-analysis.cpp
  constraint-slack-diagnostic.h
  constraint-slack-diagnostic.cpp
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
