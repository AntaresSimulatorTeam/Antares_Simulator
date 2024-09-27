/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#ifndef __SOLVER_SIMULATION_SOLVER_H__
#define __SOLVER_SIMULATION_SOLVER_H__

#include <yuni/job/queue/service.h>

#include <antares/benchmarking/DurationCollector.h>
#include <antares/logs/logs.h>
#include <antares/solver/simulation/ISimulationObserver.h>
#include <antares/study/study.h>
#include <antares/writer/writer_factory.h>
#include "antares/solver/hydro/management/management.h"
#include "antares/solver/misc/options.h"
#include "antares/solver/simulation/solver.data.h"
#include "antares/solver/simulation/solver_utils.h"
#include "antares/solver/variable/state.h"

namespace Antares::Solver::Simulation
{

template<class Impl>
class yearJob;

template<class Impl>
class ISimulation: public Impl
{
    friend class yearJob<Impl>;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor (with a given study)
    */
    ISimulation(Data::Study& study,
                const ::Settings& settings,
                Benchmarking::DurationCollector& duration_collector,
                IResultWriter& resultWriter,
                Simulation::ISimulationObserver& simulationObserver);
    //! Destructor
    ~ISimulation();
    //@}

    // Check that the writer is valid
    void checkWriter() const;

    /*!
    ** \brief Run the simulation
    */
    void run();

    /*!
    ** \brief Export the results to disk
    **
    ** \tparam ResultsForYearByYear False to write down the rglobal esults of the simulation,
    **   true for the results of the current year (year-by-year mode)
    ** \param year The current year, if applicable
    */
    void writeResults(bool synthesis, uint year = 0, uint numSpace = 9999);

    //! Reference to the current study
    Data::Study& study;
    //! The global settings
    const ::Settings& settings;

private:
    /*!
    ** \brief Regenerate time-series if required for a given year
    */
    void regenerateTimeSeries(uint year);

    /*!
    ** \brief Builds sets of parallel years
    **
    ** \return The max number of years in a set of parallel years (to be executed or not)
    */
    uint buildSetsOfParallelYears(uint firstYear,
                                  uint endYear,
                                  std::vector<setOfParallelYears>& setsOfParallelYears);

    /*!
    ** \brief Allocate storage for random numbers of parallel years
    **
    ** \param	randomParallelYears	... to be finished ...
    */
    void allocateMemoryForRandomNumbers(randomNumbers& randomForParallelYears);

    /*!
    ** \brief Computes random numbers for each years of a list
    **
    ** \param	randomForYears	Storage for random numbers for years in the list
    ** \param	years			List of years
    */
    void computeRandomNumbers(randomNumbers& randomForYears,
                              std::vector<uint>& years,
                              std::map<unsigned int, bool>& isYearPerformed,
                              MersenneTwister& randomHydro);

    /*!
    ** \brief Computes statistics on annual (system and solution) costs, to be printed in output
    *into separate files
    **
    ** Adds the contributions of each performed year contained in the current set to annual system
    *and solution costs averages over all years.
    ** These average costs are meant to be printed in output into separate files.
    ** Same thing for min and max costs over all years.
    ** Storing these costs to compute std deviation later.
    */
    void computeAnnualCostsStatistics(std::vector<Variable::State>& state,
                                      setOfParallelYears& batch);

    /*!
    ** \brief Iterate through all MC years
    **
    ** \param firstYear The first real MC year
    ** \param endYear   The last MC year
    */
    void loopThroughYears(uint firstYear, uint endYear, std::vector<Variable::State>& state);

    //! Some temporary to avoid performing useless complex checks
    Solver::Private::Simulation::CacheData pData;
    //!
    uint pNbYearsReallyPerformed;
    //! Max number of years performed in parallel
    uint pNbMaxPerformedYearsInParallel;
    //! Year by year output results
    bool pYearByYear;
    //! The first set of parallel year(s) with a performed year was already run ?
    bool pFirstSetParallelWithAPerformedYearWasRun;

    //! Statistics about annual (system and solution) costs
    annualCostsStatistics pAnnualStatistics;

    // Collecting durations inside the simulation
    Benchmarking::DurationCollector& pDurationCollector;

public:
    //! The queue service that runs every set of parallel years
    std::shared_ptr<Yuni::Job::QueueService> pQueueService = nullptr;
    //! Result writer
    Antares::Solver::IResultWriter& pResultWriter;

    std::reference_wrapper<ISimulationObserver> simulationObserver_;
}; // class ISimulation
} // namespace Antares::Solver::Simulation

#include "solver.hxx"

#endif // __SOLVER_SIMULATION_SOLVER_H__
