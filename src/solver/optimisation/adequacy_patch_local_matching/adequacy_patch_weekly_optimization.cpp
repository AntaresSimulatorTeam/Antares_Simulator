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

#include "antares/solver/optimisation/adequacy_patch_local_matching/adequacy_patch_weekly_optimization.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/simulation/simulation.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"
#include "antares/study/fwd.h"

using namespace Antares::Data::AdequacyPatch;
using Antares::Constants::nbHoursInAWeek;

namespace Antares::Solver::Optimization
{
AdequacyPatchOptimization::AdequacyPatchOptimization(const Antares::Data::Study& study,
                                                     const OptimizationOptions& options,
                                                     PROBLEME_HEBDO* problemeHebdo,
                                                     AdqPatchParams& adqPatchParams,
                                                     uint thread_number,
                                                     IResultWriter& writer) :
    WeeklyOptimization(options, problemeHebdo, adqPatchParams, thread_number, writer), study_(study)
{
}

void AdequacyPatchOptimization::solve()
{
    problemeHebdo_->adequacyPatchRuntimeData->AdequacyFirstStep = true;
    OPT_OptimisationHebdomadaire(options_, problemeHebdo_, adqPatchParams_, writer_);
    problemeHebdo_->adequacyPatchRuntimeData->AdequacyFirstStep = false;

    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; ++pays)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[pays]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            problemeHebdo_->ResultatsHoraires[pays].ValeursHorairesDENS
                = problemeHebdo_->ResultatsHoraires[pays].ValeursHorairesDeDefaillancePositive;
        else
            std::fill(problemeHebdo_->ResultatsHoraires[pays].ValeursHorairesDENS.begin(),
                    problemeHebdo_->ResultatsHoraires[pays].ValeursHorairesDENS.end(), 0);
    }

    OPT_OptimisationHebdomadaire(options_, problemeHebdo_, adqPatchParams_, writer_);
}

} // namespace Antares::Solver::Optimization
