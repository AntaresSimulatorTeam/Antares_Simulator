#
# Simulation
#
set(SRC_SIMULATION
		sim_alloc_probleme_hebdo.cpp
		include/antares/solver/simulation/sim_alloc_probleme_hebdo.h
		sim_allocation_tableaux.cpp
		sim_calcul_economique.cpp
		include/antares/solver/simulation/sim_extern_variables_globales.h
		include/antares/solver/simulation/sim_structure_donnees.h
		include/antares/solver/simulation/sim_structure_probleme_economique.h
		sim_variables_globales.cpp
		include/antares/solver/simulation/sim_constants.h
		include/antares/solver/simulation/sim_spread_generator.h
        sim_spread_generator.cpp
		include/antares/solver/simulation/simulation.h
		include/antares/solver/simulation/solver.h
		include/antares/solver/simulation/solver.hxx
		include/antares/solver/simulation/timeseries-numbers.h
		timeseries-numbers.cpp
		include/antares/solver/simulation/apply-scenario.h
		apply-scenario.cpp


		# Solver
		include/antares/solver/simulation/solver_utils.h
		solver_utils.cpp
		include/antares/solver/simulation/solver.h
		include/antares/solver/simulation/solver.hxx
		include/antares/solver/simulation/solver.data.h
		solver.data.cpp
		include/antares/solver/simulation/common-eco-adq.h
		common-eco-adq.cpp
		common-hydro-remix.cpp
		common-hydro-levels.cpp
		include/antares/solver/simulation/adequacy.h
		adequacy.cpp
		include/antares/solver/simulation/economy.h
		economy.cpp
		include/antares/solver/simulation/base_post_process.h
		base_post_process.cpp
		include/antares/solver/simulation/opt_time_writer.h
		opt_time_writer.cpp
		include/antares/solver/simulation/adequacy_patch_runtime_data.h
		adequacy_patch_runtime_data.cpp
		include/antares/solver/simulation/ITimeSeriesNumbersWriter.h
		TimeSeriesNumbersWriter.cpp
		include/antares/solver/simulation/BindingConstraintsTimeSeriesNumbersWriter.h)
source_group("simulation" FILES ${SRC_SIMULATION})

#
# --- Library VARIABLES ---
#
add_library(antares-solver-simulation
		${SRC_SIMULATION}  )

target_include_directories(antares-solver-simulation
		PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(antares-solver-simulation
		PRIVATE
		Antares::infoCollection
		PUBLIC
		yuni-static-core
		Antares::study
		Antares::result_writer
		Antares::concurrency
		Antares::solverUtils
		Antares::misc
		model_antares
		antares-solver-hydro
		antares-solver-variable
		antares-solver-ts-generator
)

install(DIRECTORY include/antares
        DESTINATION "include"
)