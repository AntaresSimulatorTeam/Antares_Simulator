set(SRC_INFEASIBLE_DIAG
  infeasible-diag/problem.cpp
  infeasible-diag/problem.h
  utils/ortools_utils.h)

add_library(libinfeasible_diag ${SRC_INFEASIBLE_DIAG})
target_link_libraries(libinfeasible_diag PUBLIC ortools::ortools sirius_solver
                                         PRIVATE libmodel_antares)
