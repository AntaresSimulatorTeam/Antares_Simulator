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
    virtual void solve(uint weekInTheYear, int hourInTheYear) = 0;
    virtual void solveCSR(Antares::Data::AreaList& areas, uint year, uint week, uint numSpace);
    static std::unique_ptr<interfaceWeeklyOptimization> create(bool adqPatchEnabled,
                                                               PROBLEME_HEBDO* problemesHebdo,
                                                               uint numSpace);

protected:
    explicit interfaceWeeklyOptimization(PROBLEME_HEBDO* problemesHebdo, uint numSpace);
    PROBLEME_HEBDO* problemeHebdo_ = nullptr;
    uint thread_number_ = 0;
};

class AdequacyPatchOptimization : public interfaceWeeklyOptimization
{
public:
    explicit AdequacyPatchOptimization(PROBLEME_HEBDO* problemeHebdo, uint numSpace);
    void solve(uint weekInTheYear, int hourInTheYear) override;
    void solveCSR(Antares::Data::AreaList& areas, uint year, uint week, uint numSpace) override;

private:
    std::vector<double> calculateENSoverAllAreasForEachHour(uint numSpace) const;
    std::set<int> identifyHoursForCurtailmentSharing(std::vector<double> sumENS, uint numSpace) const;
    std::set<int> getHoursRequiringCurtailmentSharing(uint numSpace) const;
};

class weeklyOptimization : public interfaceWeeklyOptimization
{
public:
    explicit weeklyOptimization(PROBLEME_HEBDO* problemeHebdo, uint numSpace);
    void solve(uint, int) override;
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
    std::vector<unique_ptr<interfaceWeeklyOptimization>> weeklyOptProblems_;
}; // class Economy

} // namespace Simulation
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_SIMULATION_ECONOMY_H__
