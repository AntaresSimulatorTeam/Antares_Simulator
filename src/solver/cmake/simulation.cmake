


#
# Simulation
#
set(SRC_SIMULATION
		simulation/sim_alloc_probleme_hebdo.cpp
		simulation/sim_allocation_tableaux.cpp
		simulation/sim_calcul_economique.cpp
		simulation/sim_extern_variables_globales.h
		simulation/sim_structure_donnees.h
		simulation/sim_structure_probleme_economique.h
		simulation/sim_variables_globales.cpp
        simulation/sim_constants.h
		simulation/simulation.h
		simulation/solver.h
		simulation/solver.hxx
		simulation/timeseries-numbers.h
		simulation/timeseries-numbers.cpp
		simulation/apply-scenario.h
		simulation/apply-scenario.cpp
		

		# Solver
		simulation/solver.utils.h
		simulation/solver.h
		simulation/solver.hxx
		simulation/solver.data.h
		simulation/solver.data.cpp
		simulation/common-eco-adq.h
		simulation/common-eco-adq.cpp
		simulation/common-hydro-remix.cpp
		simulation/common-hydro-levels.cpp
		simulation/adequacy.h
		simulation/adequacy.cpp
		simulation/economy.h
		simulation/economy.cpp
		simulation/base_post_process.h
		simulation/base_post_process.cpp
        simulation/opt_time_writer.h
        simulation/opt_time_writer.cpp
)
source_group("simulation" FILES ${SRC_SIMULATION})





#
# --- Library VARIABLES ---
#
add_library(libantares-solver-simulation
		${SRC_SIMULATION}  )

target_link_libraries(libantares-solver-simulation PRIVATE libantares-core)
