
add_subdirectory(remix-storage)

#
# Simulation
#
set(SRC_SIMULATION
        apply-scenario.cpp
        include/antares/solver/simulation/apply-scenario.h
        include/antares/solver/simulation/sim_alloc_probleme_hebdo.h
        include/antares/solver/simulation/sim_binding_constraints_rhs.h
        include/antares/solver/simulation/sim_constants.h
        include/antares/solver/simulation/sim_structure_donnees.h
        include/antares/solver/simulation/sim_structure_probleme_economique.h
        include/antares/solver/simulation/simulation.h
        include/antares/solver/simulation/solver.h
        include/antares/solver/simulation/solver.hxx
        include/antares/solver/simulation/numspace_manager.h
        include/antares/solver/simulation/random.h
        numspace_manager.cpp
        include/antares/solver/simulation/timeseries-numbers.h
        sim_alloc_probleme_hebdo.cpp
        sim_binding_constraints_rhs.cpp
        sim_calcul_economique.cpp
        timeseries-numbers.cpp


        # Solver
        include/antares/solver/simulation/solver_utils.h
        solver_utils.cpp
        include/antares/solver/simulation/solver.h
        include/antares/solver/simulation/solver.hxx
        include/antares/solver/simulation/simulation-run.h
        simulation-run.cpp
        include/antares/solver/simulation/common-eco-adq.h
        common-eco-adq.cpp
	    include/antares/solver/simulation/common-hydro-remix.h
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
        include/antares/solver/simulation/BindingConstraintsTimeSeriesNumbersWriter.h
        include/antares/solver/simulation/ISimulationObserver.h
        include/antares/solver/simulation/regenerate_timeseries.h
	regenerate_timeseries.cpp
        random.cpp
)
source_group("simulation" FILES ${SRC_SIMULATION})

#
# --- Library VARIABLES ---
#
add_library(antares-solver-simulation)
add_library(Antares::antares-solver-simulation ALIAS antares-solver-simulation)
target_sources(antares-solver-simulation
        PRIVATE
        ${SRC_SIMULATION})

target_include_directories(antares-solver-simulation
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(antares-solver-simulation
        PUBLIC
        Antares::infoCollection
        yuni-static-core
        Antares::study
        Antares::result_writer
        Antares::concurrency
        Antares::solverUtils
        Antares::misc
        Antares::vector-utils
        model_antares
        antares-solver-hydro
        antares-solver-variable
        antares-solver-ts-generator
		shave-peaks-by-remix-storage-gen
		Antares::simu-table-generator
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
