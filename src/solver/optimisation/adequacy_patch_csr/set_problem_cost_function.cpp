/*
** Copyright 2007-2022 RTE
** Authors: RTE-international / Redstork / Antares_Simulator Team
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

#include <algorithm>

#include "../solver/optimisation/opt_structure_probleme_a_resoudre.h"

#include "../solver/simulation/simulation.h"
#include "../solver/simulation/sim_structure_donnees.h"
#include "../simulation/adequacy_patch_runtime_data.h"

#include "../solver/optimisation/opt_fonctions.h"

namespace
{

using namespace Antares::Data::AdequacyPatch;

double calculateQuadraticCost(const PROBLEME_HEBDO* problemeHebdo, const AdqPatchPTO priceTakingOrder, int hour, int area)
{
    double priceTakingOrders = 0.0; // PTO
    if (priceTakingOrder == Data::AdequacyPatch::AdqPatchPTO::isLoad)
    {
        priceTakingOrders
          = problemeHebdo->ConsommationsAbattues[hour]->ConsommationAbattueDuPays[area]
            + problemeHebdo->AllMustRunGeneration[hour]->AllMustRunGenerationOfArea[area];
    }
    else if (priceTakingOrder == Data::AdequacyPatch::AdqPatchPTO::isDens)
    {
        priceTakingOrders = problemeHebdo->ResultatsHoraires[area]->ValeursHorairesDENS[hour];
    }

    if (priceTakingOrders <= 0.0)
        return 0.0;
    else
        return (1. / priceTakingOrders);
}
} // namespace

void HourlyCSRProblem::setQuadraticCost()
{
    const CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    CorrespondanceVarNativesVarOptim
      = problemeHebdo_->CorrespondanceVarNativesVarOptim[triggeredHour];

    // variables: ENS for each area inside adq patch
    // obj function term is: 1 / (PTO) * ENS * ENS
    //  => quadratic cost: 1 / (PTO)
    //  => linear cost: 0
    // PTO can take two different values according to option:
    //  1. from DENS
    //  2. from load
    for (int area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            int var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[area];
            if (var >= 0 && var < problemeAResoudre_.NombreDeVariables)
            {
                problemeAResoudre_.CoutQuadratique[var] = calculateQuadraticCost(problemeHebdo_, 
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
    const COUTS_DE_TRANSPORT* TransportCost;
    const CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    CorrespondanceVarNativesVarOptim
      = problemeHebdo_->CorrespondanceVarNativesVarOptim[triggeredHour];

    // variables: transmission cost for links between nodes of type 2 (area inside adequacy patch)
    // obj function term is: Sum ( hurdle_cost_direct x flow_direct )+ Sum ( hurdle_cost_indirect x
    // flow_indirect )
    //  => quadratic cost: 0
    //  => linear cost: hurdle_cost_direct or hurdle_cost_indirect
    // these members of objective functions are considered only if IntercoGereeAvecDesCouts =
    // true (use hurdle cost option is true). otherwise these members are zero.

    for (int Interco = 0; Interco < problemeHebdo_->NombreDInterconnexions; Interco++)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->originAreaMode[Interco]
              != Data::AdequacyPatch::physicalAreaInsideAdqPatch
            || problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode[Interco]
                 != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            continue;
        }
        const double coeff
          = problemeHebdo_->adequacyPatchRuntimeData->hurdleCostCoefficients[Interco];
        TransportCost = problemeHebdo_->CoutDeTransport[Interco];
        // flow
        var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
        if (var >= 0 && var < problemeAResoudre_.NombreDeVariables)
        {
            problemeAResoudre_.CoutLineaire[var] = 0.0;
            logs.debug() << var << ". Linear C = " << problemeAResoudre_.CoutLineaire[var];
        }
        // direct / indirect flow
        var = CorrespondanceVarNativesVarOptim
                ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[Interco];
        if (var >= 0 && var < problemeAResoudre_.NombreDeVariables)
        {
            if (!TransportCost->IntercoGereeAvecDesCouts)
                problemeAResoudre_.CoutLineaire[var] = 0;
            else
                problemeAResoudre_.CoutLineaire[var]
                  = TransportCost->CoutDeTransportOrigineVersExtremite[triggeredHour] * coeff;
            logs.debug() << var << ". Linear C = " << problemeAResoudre_.CoutLineaire[var];
        }

        var = CorrespondanceVarNativesVarOptim
                ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[Interco];
        if (var >= 0 && var < problemeAResoudre_.NombreDeVariables)
        {
            if (!TransportCost->IntercoGereeAvecDesCouts)
                problemeAResoudre_.CoutLineaire[var] = 0;
            else
                problemeAResoudre_.CoutLineaire[var]
                  = TransportCost->CoutDeTransportExtremiteVersOrigine[triggeredHour] * coeff;
            logs.debug() << var << ". Linear C = " << problemeAResoudre_.CoutLineaire[var];
        }
    }
}
