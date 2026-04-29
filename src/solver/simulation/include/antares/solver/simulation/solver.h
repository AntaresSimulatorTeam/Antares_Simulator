// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_SIMULATION_SOLVER_H__
#define __SOLVER_SIMULATION_SOLVER_H__

#include <stdexcept>

#include <yuni/job/queue/service.h>

#include <antares/benchmarking/DurationCollector.h>
#include <antares/logs/logs.h>
#include <antares/solver/simulation/ISimulationObserver.h>
#include <antares/study/study.h>
#include <antares/writer/writer_factory.h>
#include "antares/solver/hydro/management/management.h"
#include "antares/solver/misc/options.h"
#include "antares/solver/simulation/solver_utils.h"
#include "antares/solver/variable/state.h"

class OptimisationsSimulationTable;

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
    ** \brief Computes statistics on annual (system and solution) costs, to be printed in output
    *into separate files
    **
    ** Adds the contributions of each performed year contained in the current set to annual system
    *and solution costs averages over all years.
    ** These average costs are meant to be printed in output into separate files.
    ** Same thing for min and max costs over all years.
    ** Storing these costs to compute std deviation later.
    */
    void computeAnnualCostsStatistics(Variable::State state);

    /*!
    ** \brief Iterate through all MC years
    **
    ** \param firstYear The first real MC year
    ** \param endYear   The last MC year
    */
    void loopThroughYears(uint firstYear, uint endYear, std::vector<Variable::State>& state);

    //!
    uint pNbYearsReallyPerformed;
    //! Max number of years performed in parallel
    uint pNbMaxPerformedYearsInParallel;
    //! Year by year output results
    bool pYearByYear;

    //! Statistics about annual (system and solution) costs
    annualCostsStatistics pAnnualStatistics;

    // Collecting durations inside the simulation
    Benchmarking::DurationCollector& pDurationCollector;

    std::map<uint, std::pair<std::string, std::string>> yearSimulationBuffers_;
    std::mutex buffersMutex_;

public:
    //! The queue service that runs every set of parallel years
    std::shared_ptr<Yuni::Job::QueueService> pQueueService = nullptr;
    //! Result writer
    Antares::Solver::IResultWriter& pResultWriter;

    std::reference_wrapper<ISimulationObserver> simulationObserver_;
    void storeYearBuffers(uint year, std::string&& firstBuffer, std::string&& secondBuffer);
    void aggregateAndWriteSimulationTables();

    OptimisationsSimulationTable& getSimulationTable(uint numSpace);
}; // class ISimulation
} // namespace Antares::Solver::Simulation

#include "solver.hxx"

#endif // __SOLVER_SIMULATION_SOLVER_H__
