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

#include "../solver/optimisation/opt_structure_probleme_a_resoudre.h"

#include "../solver/simulation/simulation.h"
#include "../solver/simulation/sim_structure_donnees.h"
#include "../solver/simulation/sim_extern_variables_globales.h"

#include "../solver/optimisation/opt_fonctions.h"

double calculateQuadraticCost(PROBLEME_HEBDO* ProblemeHebdo, int hour, int area)
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
        return (1 / (priceTakingOrders * priceTakingOrders));
}

void setQuadraticCost(PROBLEME_HEBDO* ProblemeHebdo, const HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    int Var;
    int hour = hourlyCsrProblem.hourInWeekTriggeredCsr;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    const CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour];

    // variables: ENS for each area inside adq patch
    // obj function term is: 1 / (PTO * PTO) * ENS * ENS
    //  => quadratic cost: 1 / (PTO * PTO)
    //  => linear cost: 0
    // PTO can take two different values according to option:
    //  1. from DENS
    //  2. from load
    for (int area = 0; area < ProblemeHebdo->NombreDePays; ++area)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[area];
            if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutQuadratique[Var]
                  = calculateQuadraticCost(ProblemeHebdo, hour, area);
                logs.debug() << Var << ". Quad C = " << ProblemeAResoudre->CoutQuadratique[Var];
            }
        }
    }
}

void setLinearCost(PROBLEME_HEBDO* ProblemeHebdo, const HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    int Var;
    int hour = hourlyCsrProblem.hourInWeekTriggeredCsr;
    const COUTS_DE_TRANSPORT* TransportCost;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    const CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour];

    // variables: transmission cost for links between nodes of type 2 (area inside adequacy patch)
    // obj function term is: Sum ( hurdle_cost_direct x flow_direct )+ Sum ( hurdle_cost_indirect x
    // flow_indirect )
    //  => quadratic cost: 0
    //  => linear cost: hurdle_cost_direct or hurdle_cost_indirect
    // these members of objective functions are considered only if IntercoGereeAvecDesCouts =
    // OUI_ANTARES (use hurdle cost option is true). otherwise these members are zero.

    for (int Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; Interco++)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaMode[Interco]
              == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch
            && ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaMode[Interco]
                 == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            TransportCost = ProblemeHebdo->CoutDeTransport[Interco];
            // flow
            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
            if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[Var] = 0.0;
                logs.debug() << Var << ". Linear C = " << ProblemeAResoudre->CoutLineaire[Var];
            }
            // direct / indirect flow
            Var = CorrespondanceVarNativesVarOptim
                    ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[Interco];
            if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
            {
                if (TransportCost->IntercoGereeAvecDesCouts == NON_ANTARES)
                    ProblemeAResoudre->CoutLineaire[Var] = 0;
                else
                    ProblemeAResoudre->CoutLineaire[Var]
                      = TransportCost->CoutDeTransportOrigineVersExtremite[hour];
                logs.debug() << Var << ". Linear C = " << ProblemeAResoudre->CoutLineaire[Var];
            }

            Var = CorrespondanceVarNativesVarOptim
                    ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[Interco];
            if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
            {
                if (TransportCost->IntercoGereeAvecDesCouts == NON_ANTARES)
                    ProblemeAResoudre->CoutLineaire[Var] = 0;
                else
                    ProblemeAResoudre->CoutLineaire[Var]
                      = TransportCost->CoutDeTransportExtremiteVersOrigine[hour];
                logs.debug() << Var << ". Linear C = " << ProblemeAResoudre->CoutLineaire[Var];
            }
        }
    }
}

void OPT_InitialiserLesCoutsQuadratiques_CSR(PROBLEME_HEBDO* ProblemeHebdo,
                                             HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    logs.debug() << "[CSR] cost";

    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    memset((char*)ProblemeAResoudre->CoutLineaire,
           0,
           ProblemeAResoudre->NombreDeVariables * sizeof(double));

    setQuadraticCost(ProblemeHebdo, hourlyCsrProblem);
    if (ProblemeHebdo->adqPatchParams->IncludeHurdleCostCsr)
        setLinearCost(ProblemeHebdo, hourlyCsrProblem);
}