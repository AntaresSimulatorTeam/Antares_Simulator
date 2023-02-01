/*
** Copyright 2007-2023 RTE
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

#include "adequacy_patch_csr/adq_patch_curtailment_sharing.h"
#include "adequacy_patch_weekly_optimization.h"
#include "opt_fonctions.h"
#include "../simulation/simulation.h"
#include "../simulation/adequacy_patch_runtime_data.h"
#include "antares/study/area/scratchpad.h"
#include "antares/study/fwd.h"

const int nbHoursInAWeek = 168;

using namespace Antares::Data::AdequacyPatch;

namespace Antares::Solver::Optimization
{
AdequacyPatchOptimization::AdequacyPatchOptimization(PROBLEME_HEBDO* problemeHebdo,
                                                     uint thread_number) :
 WeeklyOptimization(problemeHebdo, thread_number)
{
}
void AdequacyPatchOptimization::solve(uint weekInTheYear, int hourInTheYear)
{
    problemeHebdo_->adqPatchParams->AdequacyFirstStep = true;
    OPT_OptimisationHebdomadaire(problemeHebdo_, thread_number_);
    problemeHebdo_->adqPatchParams->AdequacyFirstStep = false;

    for (int pays = 0; pays < problemeHebdo_->NombreDePays; ++pays)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[pays]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            memcpy(problemeHebdo_->ResultatsHoraires[pays]->ValeursHorairesDENS,
                   problemeHebdo_->ResultatsHoraires[pays]->ValeursHorairesDeDefaillancePositive,
                   problemeHebdo_->NombreDePasDeTemps * sizeof(double));
        else
            memset(problemeHebdo_->ResultatsHoraires[pays]->ValeursHorairesDENS,
                   0,
                   problemeHebdo_->NombreDePasDeTemps * sizeof(double));
    }

    // TODO check if we need to cut SIM_RenseignementProblemeHebdo and just pick out the
    // part that we need
    ::SIM_RenseignementProblemeHebdo(*problemeHebdo_, weekInTheYear, thread_number_, hourInTheYear);
    OPT_OptimisationHebdomadaire(problemeHebdo_, thread_number_);
}

} // namespace Antares::Solver::Optimization
