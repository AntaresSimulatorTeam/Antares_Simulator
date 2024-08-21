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
#ifndef __SOLVER_SIMULATION_ECONOMY_H__
#define __SOLVER_SIMULATION_ECONOMY_H__

#include "antares/infoCollection/StudyInfoCollector.h"
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

    bool year(Progression::Task& progression,
              Variable::State& state,
              uint numSpace,
              yearRandomNumbers& randomForYear,
              std::list<uint>& failedWeekList,
              bool isFirstPerformedYearOfSimulation,
              const HYDRO_VENTILATION_RESULTS&,
              OptimizationStatisticsWriter& optWriter,
              const Antares::Data::Area::ScratchMap& scratchmap);

    void incrementProgression(Progression::Task& progression);

    void simulationEnd();

    /*!
    ** \brief Prepare clusters in 'must-run' mode
    */
    void prepareClustersInMustRunMode(Data::Area::ScratchMap& scratchmap, uint year);

    void initializeState(Variable::State& state, uint numSpace);

private:
    uint pNbWeeks;
    uint pStartTime;
    uint pNbMaxPerformedYearsInParallel;
    std::vector<PROBLEME_HEBDO> pProblemesHebdo;
    std::vector<std::unique_ptr<Antares::Solver::Optimization::DefaultWeeklyOptimization>>
      weeklyOptProblems_;
    std::vector<std::unique_ptr<interfacePostProcessList>> postProcessesList_;
    IResultWriter& resultWriter;
    std::reference_wrapper<Simulation::ISimulationObserver> simulationObserver_;
}; // class Economy

} // namespace Antares::Solver::Simulation

#endif // __SOLVER_SIMULATION_ECONOMY_H__
