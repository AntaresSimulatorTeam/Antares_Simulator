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
#include "../solver/simulation/sim_extern_variables_globales.h"

#include "../solver/optimisation/opt_fonctions.h"

namespace
{
double calculateQuadraticCost(const PROBLEME_HEBDO* ProblemeHebdo, int hour, int area)
{
    double priceTakingOrders = 0.0; // PTO
    if (ProblemeHebdo->adqPatchParams->PriceTakingOrder == Data::AdequacyPatch::AdqPatchPTO::isLoad)
    {
        priceTakingOrders
          = ProblemeHebdo->ConsommationsAbattues[hour]->ConsommationAbattueDuPays[area]
            + ProblemeHebdo->AllMustRunGeneration[hour]->AllMustRunGenerationOfArea[area];
    }
    else if (ProblemeHebdo->adqPatchParams->PriceTakingOrder
             == Data::AdequacyPatch::AdqPatchPTO::isDens)
    {
        priceTakingOrders = ProblemeHebdo->ResultatsHoraires[area]->ValeursHorairesDENS[hour];
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

    std::fill_n(ProblemeAResoudre.CoutQuadratique,
                ProblemeAResoudre.NombreDeVariables,
                0.);

    // variables: ENS for each area inside adq patch
    // obj function term is: 1 / (PTO) * ENS * ENS
    //  => quadratic cost: 1 / (PTO)
    //  => linear cost: 0
    // PTO can take two different values according to option:
    //  1. from DENS
    //  2. from load
    for (int area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData.areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            int Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[area];
            if (Var >= 0 && Var < problemeAResoudre_.NombreDeVariables)
            {
                problemeAResoudre_.CoutQuadratique[Var]
                  = calculateQuadraticCost(problemeHebdo_, triggeredHour, area);
                logs.debug() << Var << ". Quad C = " << problemeAResoudre_.CoutQuadratique[Var];
            }
        }
    }
}

void HourlyCSRProblem::setLinearCost()
{
    int Var;
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
    // OUI_ANTARES (use hurdle cost option is true). otherwise these members are zero.

    for (int Interco = 0; Interco < problemeHebdo_->NombreDInterconnexions; Interco++)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData.originAreaMode[Interco]
              != Data::AdequacyPatch::physicalAreaInsideAdqPatch
            || problemeHebdo_->adequacyPatchRuntimeData.extremityAreaMode[Interco]
                 != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            continue;
        }

        TransportCost = problemeHebdo_->CoutDeTransport[Interco];
        // flow
        Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
        if (Var >= 0 && Var < problemeAResoudre_.NombreDeVariables)
        {
            problemeAResoudre_.CoutLineaire[Var] = 0.0;
            logs.debug() << Var << ". Linear C = " << problemeAResoudre_.CoutLineaire[Var];
        }
        // direct / indirect flow
        Var = CorrespondanceVarNativesVarOptim
                ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[Interco];
        if (Var >= 0 && Var < problemeAResoudre_.NombreDeVariables)
        {
            if (TransportCost->IntercoGereeAvecDesCouts == NON_ANTARES)
                problemeAResoudre_.CoutLineaire[Var] = 0;
            else
                problemeAResoudre_.CoutLineaire[Var]
                  = TransportCost->CoutDeTransportOrigineVersExtremite[triggeredHour];
            logs.debug() << Var << ". Linear C = " << problemeAResoudre_.CoutLineaire[Var];
        }

        Var = CorrespondanceVarNativesVarOptim
                ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[Interco];
        if (Var >= 0 && Var < problemeAResoudre_.NombreDeVariables)
        {
            if (TransportCost->IntercoGereeAvecDesCouts == NON_ANTARES)
                problemeAResoudre_.CoutLineaire[Var] = 0;
            else
                problemeAResoudre_.CoutLineaire[Var]
                  = TransportCost->CoutDeTransportExtremiteVersOrigine[triggeredHour];
            logs.debug() << Var << ". Linear C = " << problemeAResoudre_.CoutLineaire[Var];
        }
    }
}
