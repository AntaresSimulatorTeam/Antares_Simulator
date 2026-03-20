// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_SIMULATION_ECONOMY_H__
#define __SOLVER_SIMULATION_ECONOMY_H__

#include "antares/infoCollection/StudyInfoCollector.h"
#include "antares/solver/optimisation/OptimisationsSimulationTable.h"
#include "antares/solver/optimisation/weekly_optimization.h"
#include "antares/solver/simulation/opt_time_writer.h"
#include "antares/solver/simulation/solver.h" // for definition of type yearRandomNumbers
#include "antares/solver/variable/economy/all.h"
#include "antares/solver/variable/state.h"
#include "antares/solver/variable/variable.h"

#include "base_post_process.h"

namespace Antares::Solver::Simulation
{
class Economy
{
public:
    //! Name of the type of simulation
    static const char* Name()
    {
        return "economy";
    }

    static constexpr Data::SimulationMode mode = Data::SimulationMode::Economy;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    **
    ** \param study The current study
    */
    Economy(Data::Study& study,
            IResultWriter& resultWriter,
            Simulation::ISimulationObserver& simulationObserver);
    //! Destructor
    ~Economy() = default;
    //@}

    Benchmarking::OptimizationInfo getOptimizationInfo() const;

public:
    //! Current study
    Data::Study& study;
    //! All variables
    Solver::Variable::Economy::AllVariables variables;
    //! Prepro only
    bool preproOnly;

protected:
    void setNbPerformedYearsInParallel(uint nbMaxPerformedYearsInParallel);

    bool simulationBegin();

    bool year(Variable::State& state,
              uint numSpace,
              yearRandomNumbers& randomForYear,
              std::list<uint>& failedWeekList,
              const HYDRO_VENTILATION_RESULTS&,
              OptimizationStatisticsWriter& optWriter,
              Benchmarking::DurationCollector& durationCollector,
              const Antares::Data::Area::ScratchMap& scratchmap);

    void simulationEnd();

    void initializeState(Variable::State& state, uint numSpace);
    OptimisationsSimulationTable& getSimulationTable(uint numSpace);
    std::string getSimulationTableHeader() const;

private:
    uint pNbWeeks;
    uint pStartTime;
    uint pNbMaxPerformedYearsInParallel;
    std::vector<PROBLEME_HEBDO> pProblemesHebdo;
    std::vector<Optimization::WeeklyOptimization> weeklyOptProblems_;
    std::vector<std::unique_ptr<interfacePostProcessList>> postProcessesList_;
    IResultWriter& resultWriter_;
    std::reference_wrapper<Simulation::ISimulationObserver> simulationObserver_;

    std::vector<OptimisationsSimulationTable> simulationTables_;
}; // class Economy

} // namespace Antares::Solver::Simulation

#endif // __SOLVER_SIMULATION_ECONOMY_H__
