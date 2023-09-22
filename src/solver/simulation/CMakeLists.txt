#
# Simulation
#
set(SRC_SIMULATION
		sim_alloc_probleme_hebdo.cpp
		sim_alloc_probleme_hebdo.h
		sim_allocation_tableaux.cpp
		sim_calcul_economique.cpp
		sim_extern_variables_globales.h
		sim_structure_donnees.h
		sim_structure_probleme_economique.h
		sim_variables_globales.cpp
		sim_constants.h
        sim_spread_generator.h
        sim_spread_generator.cpp
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
		hydro-final-reservoir-level-functions.h
		hydro-final-reservoir-level-functions.cpp
		ITimeSeriesNumbersWriter.h
		TimeSeriesNumbersWriter.cpp
        BindingConstraintsTimeSeriesNumbersWriter.h)
source_group("simulation" FILES ${SRC_SIMULATION})

#
# --- Library VARIABLES ---
#
add_library(antares-solver-simulation
		${SRC_SIMULATION}  )

target_include_directories(antares-solver-simulation PUBLIC
		${CMAKE_SOURCE_DIR}/solver/simulation)

target_link_libraries(antares-solver-simulation PRIVATE
		Antares::infoCollection
		PUBLIC
		yuni-static-core
		Antares::study
		Antares::result_writer
)
