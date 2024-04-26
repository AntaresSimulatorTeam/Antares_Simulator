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

#include "antares/solver/optimisation/adequacy_patch_local_matching/adequacy_patch_weekly_optimization.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"
#include "antares/study/fwd.h"
#include "antares/solver/simulation/ISimulationObserver.h"

using namespace Antares::Data::AdequacyPatch;
using Antares::Constants::nbHoursInAWeek;

namespace Antares::Solver::Optimization
{
AdequacyPatchOptimization::AdequacyPatchOptimization(const Antares::Data::Study& study,
                                                     const OptimizationOptions& options,
                                                     PROBLEME_HEBDO* problemeHebdo,
                                                     AdqPatchParams& adqPatchParams,
                                                     uint thread_number,
                                                     IResultWriter& writer,
                                                     Simulation::ISimulationObserver& observer)
 : WeeklyOptimization(options, problemeHebdo, adqPatchParams, thread_number, writer, observer)
 , study_(study)
{
}

void AdequacyPatchOptimization::solve()
{
    auto nullSimulationObserver = std::make_unique<Simulation::NullSimulationObserver>();
    problemeHebdo_->adequacyPatchRuntimeData->AdequacyFirstStep = true;
    OPT_OptimisationHebdomadaire(options_, problemeHebdo_, adqPatchParams_, writer_, *nullSimulationObserver);
    problemeHebdo_->adequacyPatchRuntimeData->AdequacyFirstStep = false;

    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; ++pays)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[pays]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            problemeHebdo_->ResultatsHoraires[pays].ValeursHorairesDENS
                = problemeHebdo_->ResultatsHoraires[pays].ValeursHorairesDeDefaillancePositive;
        else
            std::ranges::fill(problemeHebdo_->ResultatsHoraires[pays].ValeursHorairesDENS, 0);
    }

    OPT_OptimisationHebdomadaire(options_, problemeHebdo_, adqPatchParams_, writer_, *nullSimulationObserver);
}

} // namespace Antares::Solver::Optimization
