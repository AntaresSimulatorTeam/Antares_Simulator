// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_SIMULATION_ADEQUACY_H__
#define __SOLVER_SIMULATION_ADEQUACY_H__

#include "antares/infoCollection/StudyInfoCollector.h"
#include "antares/io/outputs/SimulationTable.h"
#include "antares/solver/simulation/common-eco-adq.h"
#include "antares/solver/simulation/opt_time_writer.h"
#include "antares/solver/simulation/solver.h" // for definition of type yearRandomNumbers
#include "antares/solver/variable/adequacy/all.h"
#include "antares/solver/variable/economy/all.h"
#include "antares/solver/variable/state.h"
#include "antares/solver/variable/variable.h"

namespace Antares::Solver::Simulation
{
class Adequacy
{
public:
    //! Name of the type of simulation
    static const char* Name()
    {
        return "adequacy";
    }

    static constexpr Data::SimulationMode mode = Data::SimulationMode::Adequacy;

    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    **
    ** \param study The current study
    */
    Adequacy(Data::Study& study,
             IResultWriter& resultWriter,
             Simulation::ISimulationObserver& simulationObserver);
    //! Destructor
    ~Adequacy() = default;
    //@}

    Benchmarking::OptimizationInfo getOptimizationInfo() const;

    //! Current study
    Data::Study& study;
    //! All variables
    Solver::Variable::Adequacy::AllVariables variables;
    //! Prepro only
    bool preproOnly = false;

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

private:
    bool simplexIsRequired(uint hourInTheYear,
                           uint numSpace,
                           const HYDRO_VENTILATION_RESULTS&) const;

    uint pNbWeeks;
    uint pStartTime;
    uint pNbMaxPerformedYearsInParallel;
    std::vector<PROBLEME_HEBDO> pProblemesHebdo;
    Matrix<> pRES;
    IResultWriter& resultWriter;
    std::reference_wrapper<Simulation::ISimulationObserver> simulationObserver_;
}; // class Adequacy

} // namespace Antares::Solver::Simulation

#endif // __SOLVER_SIMULATION_ADEQUACY_H__
