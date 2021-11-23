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

#include <yuni/yuni.h>
#include <antares/study/memory-usage.h>
#include "adequacy-draft.h"
#include <antares/study.h>
#include "simulation.h"
#include "../optimisation/opt_fonctions.h"
#include "../aleatoire/alea_fonctions.h"

namespace Antares
{
namespace Solver
{
namespace Simulation
{
AdequacyDraft::AdequacyDraft(Data::Study& study) : study(study), preproOnly(false)
{
}

AdequacyDraft::~AdequacyDraft()
{
    SIM_DesallocationProblemeHoraireAdequation();
}

void AdequacyDraft::setNbPerformedYearsInParallel(uint nbMaxPerformedYearsInParallel)
{
    pNbMaxPerformedYearsInParallel = nbMaxPerformedYearsInParallel;
}

void AdequacyDraft::initializeState(Variable::State& state, uint)
{
    state.simplexHasBeenRan = false;
}

bool AdequacyDraft::simulationBegin()
{
    SIM_InitialisationProblemeHoraireAdequation();

    pStartHour = study.calendar.days[study.parameters.simulationDays.first].hours.first;
    pFinalHour = study.calendar.days[study.parameters.simulationDays.end - 1].hours.end;

    pYearsRatio = 1. / (double)study.runtime->rangeLimits.year[Data::rangeCount];

    return true;
}

bool AdequacyDraft::year(Progression::Task& progression,
                         Variable::State& state,
                         uint numSpace,
                         yearRandomNumbers&,
                         std::list<uint>&,
                         bool)
{
    SIM_RenseignementValeursPourTouteLAnnee(study, numSpace);

    state.startANewYear();

    for (unsigned int hour = pStartHour; hour != pFinalHour; ++hour)
    {
        state.hourInTheYear = (unsigned int)hour;
        state.eensSystemIS = 0.;
        state.eensSystemCN = 0.;

        SIM_RenseignementProblemeHoraireAdequation(hour);
        SIM_CalculFlotHoraireAdequation();

        variables.hourBegin(state.hourInTheYear);

        variables.hourForEachArea(state, numSpace);

        variables.hourEnd(state, state.hourInTheYear);

        ++state.hourInTheSimulation;
    }

    ++progression;
    return true;
}

void AdequacyDraft::incrementProgression(Progression::Task& progression)
{
    ++progression;
}

void AdequacyDraft::simulationEnd()
{
}

void AdequacyDraft::prepareClustersInMustRunMode(uint)
{
}

} // namespace Simulation
} // namespace Solver
} // namespace Antares
