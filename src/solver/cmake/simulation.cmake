


#
# Simulation
#
set(SRC_SIMULATION
		simulation/sim_algorithme_de_flot.cpp
		simulation/sim_alloc_probleme_hebdo.cpp
		simulation/sim_allocation_tableaux.cpp
		simulation/sim_calcul_adequation.cpp
		simulation/sim_calcul_economique.cpp
		simulation/sim_extern_variables_globales.h
		simulation/sim_structure_donnees.h
		simulation/sim_structure_probleme_adequation.h
		simulation/sim_structure_probleme_economique.h
		simulation/sim_variables_globales.cpp
		simulation/simulation.h
		simulation/solver.h
		simulation/solver.hxx
		simulation/timeseries-numbers.h
		simulation/timeseries-numbers.cpp

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
		simulation/common-dispatchable-margin.cpp
		simulation/adequacy-draft.h
		simulation/adequacy-draft.cpp
		simulation/adequacy.h
		simulation/adequacy.cpp
		simulation/economy.h
		simulation/economy.cpp
)
source_group("simulation" FILES ${SRC_SIMULATION})





#
# --- Library VARIABLES ---
#
add_library(libantares-solver-simulation
		${SRC_SIMULATION}  )

add_library(libantares-solver-simulation-swap
		${SRC_SIMULATION}  )

set_target_properties(libantares-solver-simulation-swap PROPERTIES COMPILE_FLAGS " -DANTARES_SWAP_SUPPORT=1")

