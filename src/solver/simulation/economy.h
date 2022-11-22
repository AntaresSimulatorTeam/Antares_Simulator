/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __SOLVER_SIMULATION_ECONOMY_H__
#define __SOLVER_SIMULATION_ECONOMY_H__

#include <yuni/yuni.h>
#include <memory>
#include <antares/benchmarking.h>
#include "../variable/variable.h"
#include "../variable/economy/all.h"
#include "../variable/state.h"
#include "common-eco-adq.h"

#include "solver.h" // for definition of type yearRandomNumbers

namespace Antares
{
namespace Solver
{
namespace Simulation
{
class interfaceWeeklyOptimization
{
public:
    virtual void solve(uint weekInTheYear, int hourInTheYear, uint numSpace) = 0;
    virtual void solveCSR(const Variable::State& state, uint numSpace, uint week) = 0;
    static std::unique_ptr<interfaceWeeklyOptimization> create(bool adqPatchEnabled, PROBLEME_HEBDO** pProblemesHebdo);

protected:
    explicit interfaceWeeklyOptimization(PROBLEME_HEBDO** pProblemesHebdo);
    PROBLEME_HEBDO** pProblemesHebdo;
};

class AdequacyPatchOptimization : public interfaceWeeklyOptimization
{
public:
    explicit AdequacyPatchOptimization(PROBLEME_HEBDO** problemesHebdo);
    void solve(uint weekInTheYear, int hourInTheYear, uint numSpace) override;
    void solveCSR(const Variable::State& state, uint numSpace, uint week) override;

private:
    vector<double> calculateENSoverAllAreasForEachHour(uint numSpace) const;
    std::set<int> identifyHoursForCurtailmentSharing(vector<double> sumENS, uint numSpace) const;
    std::set<int> getHoursRequiringCurtailmentSharing(uint numSpace) const;
};

class weeklyOptimization : public interfaceWeeklyOptimization
{
public:
    explicit weeklyOptimization(PROBLEME_HEBDO** problemesHebdo);
    void solve(uint, int, uint numSpace) override;
    void solveCSR(const Variable::State& state, uint numSpace, uint week) override;
};

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
    Economy(Data::Study& study);
    //! Destructor
    ~Economy();
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
              bool isFirstPerformedYearOfSimulation);

    void incrementProgression(Progression::Task& progression);

    void simulationEnd();

    /*!
    ** \brief Prepare clusters in 'must-run' mode
    */
    void prepareClustersInMustRunMode(uint numSpace);

    void initializeState(Variable::State& state, uint numSpace);

private:
    uint pNbWeeks;
    uint pStartTime;
    uint pNbMaxPerformedYearsInParallel;
    bool pPreproOnly;
    PROBLEME_HEBDO** pProblemesHebdo;
    std::unique_ptr<interfaceWeeklyOptimization> weeklyOptProblem;
}; // class Economy

} // namespace Simulation
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_SIMULATION_ECONOMY_H__
