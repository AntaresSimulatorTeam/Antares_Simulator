#
# Simulation
#
set(SRC_SIMULATION
		sim_alloc_probleme_hebdo.cpp
		sim_allocation_tableaux.cpp
		sim_calcul_economique.cpp
		sim_extern_variables_globales.h
		sim_structure_donnees.h
		sim_structure_probleme_adequation.h
		sim_structure_probleme_economique.h
		sim_variables_globales.cpp
		sim_constants.h
        sim_st_storage_draw_spread.h
        sim_st_storage_draw_spread.cpp
		simulation.h
		solver.h
		solver.hxx
		timeseries-numbers.h
		timeseries-numbers.cpp
		apply-scenario.h
		apply-scenario.cpp


		# Solver
		solver.utils.h
		solver.h
		solver.hxx
		solver.data.h
		solver.data.cpp
		common-eco-adq.h
		common-eco-adq.cpp
		common-hydro-remix.cpp
		common-hydro-levels.cpp
		adequacy.h
		adequacy.cpp
		economy.h
		economy.cpp
		base_post_process.h
		base_post_process.cpp
		opt_time_writer.h
		opt_time_writer.cpp
		adequacy_patch_runtime_data.h
		adequacy_patch_runtime_data.cpp
		)
source_group("simulation" FILES ${SRC_SIMULATION})





#
# --- Library VARIABLES ---
#
add_library(libantares-solver-simulation
		${SRC_SIMULATION}  )

target_include_directories(libantares-solver-simulation PUBLIC
		${CMAKE_SOURCE_DIR}/solver/simulation)

target_link_libraries(libantares-solver-simulation PRIVATE libantares-core)
