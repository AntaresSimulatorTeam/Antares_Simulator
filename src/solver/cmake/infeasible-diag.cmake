set(SRC_INFEASIBLE_DIAG
  infeasible-diag/problem.cpp
  infeasible-diag/problem.h
  infeasible-diag/report.h
  infeasible-diag/report.cpp
  )

add_library(libinfeasible_diag ${SRC_INFEASIBLE_DIAG})
target_link_libraries(libinfeasible_diag
  PUBLIC ortools::ortools sirius_solver
  PRIVATE libantares-core # antares/study/OrtoolsSolver.cpp
  )
