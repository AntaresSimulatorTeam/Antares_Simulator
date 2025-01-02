include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

# Useful variables definitions
set(src_solver_optimisation "${CMAKE_SOURCE_DIR}/solver/optimisation")

add_boost_test(tests-adq-patch
  SRC adequacy_patch.cpp
  INCLUDE "${src_solver_optimisation}"
  LIBS
  model_antares
  array)
