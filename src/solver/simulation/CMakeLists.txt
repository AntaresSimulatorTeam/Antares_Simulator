# Remix hydro algorithm
add_library(shave-peaks-by-remix-hydro)

target_sources(shave-peaks-by-remix-hydro
        PRIVATE
        shave-peaks-by-remix-hydro.cpp)

target_include_directories(shave-peaks-by-remix-hydro
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

#
# Simulation
#
set(SRC_SIMULATION
        sim_alloc_probleme_hebdo.cpp
        include/antares/solver/simulation/sim_alloc_probleme_hebdo.h
        sim_calcul_economique.cpp
        include/antares/solver/simulation/sim_structure_donnees.h
        include/antares/solver/simulation/sim_structure_probleme_economique.h
        include/antares/solver/simulation/sim_constants.h
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
        include/antares/solver/simulation/simulation-run.h
        simulation-run.cpp
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
        include/antares/solver/simulation/BindingConstraintsTimeSeriesNumbersWriter.h
        include/antares/solver/simulation/ISimulationObserver.h
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
        model_antares
        antares-solver-hydro
        antares-solver-variable
        antares-solver-ts-generator
        shave-peaks-by-remix-hydro
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
