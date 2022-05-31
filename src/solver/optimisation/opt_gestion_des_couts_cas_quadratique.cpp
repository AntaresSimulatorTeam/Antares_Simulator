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

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"

void OPT_InitialiserLesCoutsQuadratiques(PROBLEME_HEBDO* ProblemeHebdo, int PdtHebdo)
{
    int Interco;
    int Var;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeResistances;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;

    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;

    memset((char*)ProblemeAResoudre->CoutLineaire,
           0,
           ProblemeAResoudre->NombreDeVariables * sizeof(double));
    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[0];
    ValeursDeResistances = ProblemeHebdo->ValeursDeNTC[PdtHebdo];

    for (Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; Interco++)
    {
        Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
        if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
            ProblemeAResoudre->CoutQuadratique[Var]
              = ValeursDeResistances->ResistanceApparente[Interco];
    }
}

void OPT_InitialiserLesCoutsQuadratiques_CSR(PROBLEME_HEBDO* ProblemeHebdo,
                                             HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    logs.debug() << "[CSR] cost";
    int Var;
    int hour = hourlyCsrProblem.hourInWeekTriggeredCsr;
    double priceTakingOrders; // PTO
    double quadraticCost;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    int Interco;
    COUTS_DE_TRANSPORT* TransportCost;

    // variables: ENS for each area inside adq patch
    // obj function term is: 1 / (PTO * PTO) * ENS * ENS
    //  => quadratic cost: 1 / (PTO * PTO)
    //  => linear cost: 0
    // PTO can take two different values according to option:
    //  1. from DENS
    //  2. from load
    memset((char*)ProblemeAResoudre->CoutLineaire,
           0,
           ProblemeAResoudre->NombreDeVariables * sizeof(double));
    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour];

    for (int area = 0; area < ProblemeHebdo->NombreDePays; ++area)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[area];
            if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
            {
                if (ProblemeHebdo->adqPatch->PriceTakingOrder
                    == Data::AdequacyPatch::AdqPatchPTO::isLoad)
                {
                    priceTakingOrders
                      = ProblemeHebdo->ConsommationsAbattues[hour]->ConsommationAbattueDuPays[area]
                        + ProblemeHebdo->AllMustRunGeneration[hour]
                            ->AllMustRunGenerationOfArea[area];
                }
                else
                {
                    priceTakingOrders
                      = ProblemeHebdo->ResultatsHoraires[area]->ValeursHorairesDENS[hour];
                }

                if (priceTakingOrders <= 0.0)
                {
                    // CSR todo a warning that DENS is negative and it is considered for CSR, there
                    // was a check for positive threshold
                    quadraticCost = 0.0;
                }
                else
                {
                    quadraticCost = 1 / (priceTakingOrders * priceTakingOrders);
                }

                ProblemeAResoudre->CoutQuadratique[Var] = quadraticCost;
                logs.debug() << Var << ". Quad C = " << ProblemeAResoudre->CoutQuadratique[Var];
            }
        }
    }

    // variables: transmission cost for links between nodes of type 2 (area inside adequacy patch)
    // obj function term is: Sum ( hurdle_cost_direct x flow_direct )+ Sum ( hurdle_cost_indirect x
    // flow_indirect )
    //  => quadratic cost: 0
    //  => linear cost: hurdle_cost_direct or hurdle_cost_indirect
    // these members of objective functions are considered only if IntercoGereeAvecDesCouts =
    // OUI_ANTARES (use hurdle cost option is true). otherwise these members are zero.

    for (Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; Interco++)
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