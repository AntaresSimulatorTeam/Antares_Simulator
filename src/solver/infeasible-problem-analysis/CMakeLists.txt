project(infeasible-problem-analysis)

set(SRC_INFEASIBLE_PROBLEM_ANALYSIS
  unfeasibility-analysis.h
  constraint-slack-analysis.h
  constraint-slack-analysis.cpp
  variables-bounds-consistency.h
  variables-bounds-consistency.cpp
  unfeasible-pb-analyzer.cpp
  unfeasible-pb-analyzer.h
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
target_include_directories(infeasible_problem_analysis
  PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)
