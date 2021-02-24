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
#ifndef __SOLVER_SIMULATION_ADEQUACY_DRAFT_H__
#define __SOLVER_SIMULATION_ADEQUACY_DRAFT_H__

#include "../variable/variable.h"
#include "../variable/adequacy-draft/all.h"
#include "../variable/state.h"

#include "solver.h"

namespace Antares
{
namespace Solver
{
namespace Simulation
{
class AdequacyDraft
{
public:
    //! Name of the type of simulation
    static const char* Name()
    {
        return "adequacy-draft";
    }

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    **
    ** \param study The current study
    */
    AdequacyDraft(Data::Study& study);
    //! Destructor
    ~AdequacyDraft();
    //@}

public:
    //! Current study
    Data::Study& study;
    //! All variables
    Solver::Variable::AdequacyDraft::AllVariables variables;
    //! Prepro only
    bool preproOnly;

protected:
    void setNbPerformedYearsInParallel(uint nbMaxPerformedYearsInParallel);

    bool simulationBegin();
    bool year(Progression::Task& progression,
              Variable::State& state,
              uint numSpace,
              yearRandomNumbers& randomForYear,
              std::list<uint>& failedWeekList);
    void simulationEnd();

    void incrementProgression(Progression::Task& progression);

    /*!
    ** \brief Prepare clusters in 'must-run' mode
    */
    void prepareClustersInMustRunMode(uint numSpace);

    void initializeState(Variable::State& state, uint numSpace);

private:
    uint pStartHour;
    uint pFinalHour;
    double pYearsRatio;
    uint pNbMaxPerformedYearsInParallel;

}; // class AdequacyDraft

} // namespace Simulation
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_SIMULATION_ADEQUACY_DRAFT_H__
