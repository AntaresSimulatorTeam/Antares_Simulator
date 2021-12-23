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
#ifndef __SOLVER_SIMULATION_ADEQUACY_H__
#define __SOLVER_SIMULATION_ADEQUACY_H__

#include <yuni/yuni.h>
#include "../variable/variable.h"
#include "../variable/adequacy/all.h"
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
class Adequacy
{
public:
    //! Name of the type of simulation
    static const char* Name()
    {
        return "adequacy";
    }

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    **
    ** \param study The current study
    */
    Adequacy(Data::Study& study);
    //! Destructor
    ~Adequacy();
    //@}

public:
    //! Current study
    Data::Study& study;
    //! All variables
    Solver::Variable::Adequacy::AllVariables variables;
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
    AvgExchangeResults* callbackRetrieveBalanceData(Data::Area* area);
    bool simplexIsRequired(uint hourInTheYear, uint numSpace) const;

private:
    uint pNbWeeks;
    uint pStartTime;
    uint pNbMaxPerformedYearsInParallel;
    bool pPreproOnly;
    PROBLEME_HEBDO** pProblemesHebdo;
    Matrix<> pRES;

}; // class Adequacy

} // namespace Simulation
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_SIMULATION_ADEQUACY_H__
