/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace
{

using namespace Antares::Data::AdequacyPatch;

double calculateQuadraticCost(const PROBLEME_HEBDO* problemeHebdo,
                              const AdqPatchPTO priceTakingOrder,
                              int hour,
                              int area)
{
    using namespace Data::AdequacyPatch;
    double priceTakingOrders = 0.0; // PTO
    if (priceTakingOrder == AdqPatchPTO::isLoad)
    {
        priceTakingOrders = problemeHebdo->ConsommationsAbattues[hour]
                              .ConsommationAbattueDuPays[area]
                            + problemeHebdo->AllMustRunGeneration[hour]
                                .AllMustRunGenerationOfArea[area];
    }
    else // AdqPatchPTO::isDens
    {
        priceTakingOrders = problemeHebdo->ResultatsHoraires[area].ValeursHorairesDENS[hour];
    }

    if (priceTakingOrders <= 0.0)
    {
        return 0.0;
    }
    else
    {
        return (1. / priceTakingOrders);
    }
}
} // namespace

void HourlyCSRProblem::setQuadraticCost()
{
    // variables: ENS for each area inside adq patch
    // obj function term is: 1 / (PTO) * ENS * ENS
    //  => quadratic cost: 1 / (PTO)
    //  => linear cost: 0
    // PTO can take two different values according to option:
    //  1. from DENS
    //  2. from load
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            int var = variableManager_.PositiveUnsuppliedEnergy(area, triggeredHour);
            if (var >= 0 && var < problemeAResoudre_.NombreDeVariables)
            {
                problemeAResoudre_.CoutQuadratique[var] = calculateQuadraticCost(
                  problemeHebdo_,
                  adqPatchParams_.curtailmentSharing.priceTakingOrder,
                  triggeredHour,
                  area);
                logs.debug() << var << ". Quad C = " << problemeAResoudre_.CoutQuadratique[var];
            }
        }
    }
}

void HourlyCSRProblem::setLinearCost()
{
    int var;

    // variables: transmission cost for links between nodes of type 2 (area inside adequacy patch)
    // obj function term is: Sum ( hurdle_cost_direct x flow_direct )+ Sum ( hurdle_cost_indirect x
    // flow_indirect )
    //  => quadratic cost: 0
    //  => linear cost: hurdle_cost_direct or hurdle_cost_indirect
    // these members of objective functions are considered only if IntercoGereeAvecDesCouts =
    // true (use hurdle cost option is true). otherwise these members are zero.

    for (uint32_t Interco = 0; Interco < problemeHebdo_->NombreDInterconnexions; Interco++)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->originAreaMode[Interco]
              != Data::AdequacyPatch::physicalAreaInsideAdqPatch
            || problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode[Interco]
                 != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            continue;
        }
        const double coeff = problemeHebdo_->adequacyPatchRuntimeData
                               ->hurdleCostCoefficients[Interco];

        const COUTS_DE_TRANSPORT& TransportCost = problemeHebdo_->CoutDeTransport[Interco];
        // flow
        var = variableManager_.NTCDirect(Interco, triggeredHour);
        if (var >= 0 && var < problemeAResoudre_.NombreDeVariables)
        {
            problemeAResoudre_.CoutLineaire[var] = 0.0;
            logs.debug() << var << ". Linear C = " << problemeAResoudre_.CoutLineaire[var];
        }
        // direct / indirect flow
        var = variableManager_.IntercoDirectCost(Interco, triggeredHour);
        if (var >= 0 && var < problemeAResoudre_.NombreDeVariables)
        {
            if (!TransportCost.IntercoGereeAvecDesCouts)
            {
                problemeAResoudre_.CoutLineaire[var] = 0;
            }
            else
            {
                problemeAResoudre_.CoutLineaire[var] = TransportCost
                                                         .CoutDeTransportOrigineVersExtremite
                                                           [triggeredHour]
                                                       * coeff;
            }
            logs.debug() << var << ". Linear C = " << problemeAResoudre_.CoutLineaire[var];
        }

        var = variableManager_.IntercoIndirectCost(Interco, triggeredHour);
        if (var >= 0 && var < problemeAResoudre_.NombreDeVariables)
        {
            if (!TransportCost.IntercoGereeAvecDesCouts)
            {
                problemeAResoudre_.CoutLineaire[var] = 0;
            }
            else
            {
                problemeAResoudre_.CoutLineaire[var] = TransportCost
                                                         .CoutDeTransportExtremiteVersOrigine
                                                           [triggeredHour]
                                                       * coeff;
            }
            logs.debug() << var << ". Linear C = " << problemeAResoudre_.CoutLineaire[var];
        }
    }
}
