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

#include "pi_constantes_externes.h"

#include <math.h>
#include <yuni/core/math.h>

#define ZERO_POUR_LES_VARIABLES_FIXES 1.e-6

using namespace Yuni;

void OPT_InitialiserLesBornesDesVariablesDuProblemeQuadratique(PROBLEME_HEBDO* ProblemeHebdo,
                                                               int PdtHebdo)
{
    int Interco;
    int Var;
    double* AdresseDuResultat;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeNTC;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;

    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;

    for (Var = 0; Var < ProblemeAResoudre->NombreDeVariables; Var++)
        ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = NULL;

    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[0];
    ValeursDeNTC = ProblemeHebdo->ValeursDeNTC[PdtHebdo];

    for (Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; Interco++)
    {
        Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
        ProblemeAResoudre->Xmax[Var] = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco];
        ProblemeAResoudre->Xmin[Var] = -(ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]);

        if (ProblemeAResoudre->Xmax[Var] - ProblemeAResoudre->Xmin[Var]
            < ZERO_POUR_LES_VARIABLES_FIXES)
        {
            ProblemeAResoudre->X[Var]
              = 0.5 * (ProblemeAResoudre->Xmax[Var] - ProblemeAResoudre->Xmin[Var]);
            ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_FIXE;
        }
        else
        {
            if (Math::Infinite(ProblemeAResoudre->Xmax[Var]) == 1)
            {
                if (Math::Infinite(ProblemeAResoudre->Xmin[Var]) == -1)
                    ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_NON_BORNEE;
                else
                    ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
            }
            else
            {
                if (Math::Infinite(ProblemeAResoudre->Xmin[Var]) == -1)
                    ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_SUPERIEUREMENT;
                else
                    ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
            }
        }
        AdresseDuResultat = &(ValeursDeNTC->ValeurDuFlux[Interco]);
        ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = AdresseDuResultat;
    }
}

void calculateDensNew(PROBLEME_HEBDO* ProblemeHebdo, HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    double netPositionInit;
    double flowsNode1toNodeA;
    double densNew;
    double ensInit;
    int hour = hourlyCsrProblem.hourInWeekTriggeredCsr;
    int Interco;
    bool includeFlowsOutsideAdqPatchToDensNew
      = !ProblemeHebdo->adqPatch->LinkCapacityForAdqPatchFirstStepFromAreaOutsideToAreaInsideAdq;

    for (int Area = 0; Area < ProblemeHebdo->NombreDePays; Area++)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[Area]
            == Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
        {
            netPositionInit = 0;
            flowsNode1toNodeA = 0;

            Interco = ProblemeHebdo->IndexDebutIntercoOrigine[Area];
            while (Interco >= 0)
            {
                if (ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaType[Interco]
                    == Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
                {
                    netPositionInit -= ProblemeHebdo->ValeursDeNTC[hour]->ValeurDuFlux[Interco];
                }
                else if (ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaType[Interco]
                         == Data::AdequacyPatch::adqmPhysicalAreaOutsideAdqPatch)
                {
                    flowsNode1toNodeA
                      -= Math::Min(0, ProblemeHebdo->ValeursDeNTC[hour]->ValeurDuFlux[Interco]);
                }
                Interco = ProblemeHebdo->IndexSuivantIntercoOrigine[Interco];
            }
            Interco = ProblemeHebdo->IndexDebutIntercoExtremite[Area];
            while (Interco >= 0)
            {
                if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaType[Interco]
                    == Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
                {
                    netPositionInit += ProblemeHebdo->ValeursDeNTC[hour]->ValeurDuFlux[Interco];
                }
                else if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaType[Interco]
                         == Data::AdequacyPatch::adqmPhysicalAreaOutsideAdqPatch)
                {
                    flowsNode1toNodeA
                      += Math::Max(0, ProblemeHebdo->ValeursDeNTC[hour]->ValeurDuFlux[Interco]);
                }
                Interco = ProblemeHebdo->IndexSuivantIntercoExtremite[Interco];
            }
            // calculate densNew per area
            ensInit
              = ProblemeHebdo->ResultatsHoraires[Area]->ValeursHorairesDeDefaillancePositive[hour];
            if (includeFlowsOutsideAdqPatchToDensNew)
            {
                densNew = Math::Max(0, ensInit + netPositionInit + flowsNode1toNodeA);
            }
            else
            {
                densNew = Math::Max(0, ensInit + netPositionInit);
            }
            hourlyCsrProblem.densNewValues.insert(std::pair<int, double>(Area, densNew));
            // todo can we use vector, is it going to be the same order when we pass it to Xmax!
            // or it's safer to go with the key(area)-value map?
        }
    }
    return;
}

void OPT_InitialiserLesBornesDesVariablesDuProblemeQuadratique_CSR(
  PROBLEME_HEBDO* ProblemeHebdo,
  HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    logs.debug() << "[CSR] bounds";

    int Var;
    double* AdresseDuResultat;
    int hour;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;

    hour = hourlyCsrProblem.hourInWeekTriggeredCsr;
    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;

    for (Var = 0; Var < ProblemeAResoudre->NombreDeVariables; Var++)
        ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = NULL;

    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour];

    // variables: ENS for each area inside adq patch
    // one dummy constraint for testing : 0 <= ENS <= 10000 //todo remove
    for (int area = 0; area < ProblemeHebdo->NombreDePays; ++area)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[area] == Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
        {
            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[area];

            ProblemeAResoudre->Xmin[Var] = 0;
            ProblemeAResoudre->Xmax[Var] = 10000; // densNew should be bound here!

            if (Math::Infinite(ProblemeAResoudre->Xmax[Var]) == 1)
            {
                if (Math::Infinite(ProblemeAResoudre->Xmin[Var]) == -1)
                    ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_NON_BORNEE;
                else
                    ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
            }
            else
            {
                if (Math::Infinite(ProblemeAResoudre->Xmin[Var]) == -1)
                    ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_SUPERIEUREMENT;
                else
                    ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
            }

            ProblemeHebdo->ResultatsHoraires[area]->ValeursHorairesDeDefaillancePositive[hour] = 0.0;
            AdresseDuResultat = &(ProblemeHebdo->ResultatsHoraires[area]->ValeursHorairesDeDefaillancePositive[hour]);

            ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = AdresseDuResultat;

            logs.debug() << Var << ": " << ProblemeAResoudre->Xmin[Var] << ", " << ProblemeAResoudre->Xmax[Var];
        }
    }

    // variables: Spilled Energy for each area inside adq patch // todo after debugging transfer this into same loop as ENS
    for (int area = 0; area < ProblemeHebdo->NombreDePays; ++area)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[area] == Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
        {
            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[area];

            ProblemeAResoudre->Xmin[Var] = 0.0;
            ProblemeAResoudre->Xmax[Var] = LINFINI_ANTARES;

            // if (Math::Infinite(ProblemeAResoudre->Xmax[Var]) == 1)
            // {
            //     if (Math::Infinite(ProblemeAResoudre->Xmin[Var]) == -1)
            //         ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_NON_BORNEE;
            //     else
            //         ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
            // }
            // else
            // {
            //     if (Math::Infinite(ProblemeAResoudre->Xmin[Var]) == -1)
            //         ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_SUPERIEUREMENT;
            //     else
            //         ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
            // } // no need for this check here!

            ProblemeHebdo->ResultatsHoraires[area]->ValeursHorairesDeDefaillanceNegative[hour] = 0.0;
            AdresseDuResultat = &(ProblemeHebdo->ResultatsHoraires[area]->ValeursHorairesDeDefaillanceNegative[hour]);

            ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = AdresseDuResultat;

            logs.debug() << Var << ": " << ProblemeAResoudre->Xmin[Var] << ", " << ProblemeAResoudre->Xmax[Var];
        }
    }

    // variables bounds: transmissin flows (flow, direct_direct and flow_indirect). For links between nodes of type 2. 
    // Set hourly bounds for links between nodes of type 2, depending on the user input (max direct and indirect flow).
    double* Xmin;
    double* Xmax;
    int* TypeDeVariable;    
    VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeNTC;
    COUTS_DE_TRANSPORT* TransportCost;
    Xmin = ProblemeAResoudre->Xmin;
    Xmax = ProblemeAResoudre->Xmax;

    ValeursDeNTC = ProblemeHebdo->ValeursDeNTC[hour];

    for (int Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; ++Interco)
    {

        // only consider link between 2 and 2
        if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaType[Interco] == Antares::Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch
        && ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaType[Interco] == Antares::Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
        {
            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
            TransportCost = ProblemeHebdo->CoutDeTransport[Interco];

            Xmax[Var] = 2000; //ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco];
            Xmin[Var] = 0; //-(ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]);

            if (Math::Infinite(Xmax[Var]) == 1)
            {
                if (Math::Infinite(Xmin[Var]) == -1)
                    ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_NON_BORNEE;
                else
                    ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
            }
            else
            {
                if (Math::Infinite(Xmin[Var]) == -1)
                    ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_SUPERIEUREMENT;
                else
                    ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
            }

            // AdresseDuResultat = &(ProblemeHebdo->VariablesDualesDesContraintesDeNTC[hour]
            //                         ->VariableDualeParInterconnexion[Interco]); // dual variable for links // todo: remove
            // ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsReduits[Var] = AdresseDuResultat; // reduced cost // todo: remove

            AdresseDuResultat = &(ValeursDeNTC->ValeurDuFlux[Interco]);
            ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = AdresseDuResultat;

            logs.debug() << Var << ": " << ProblemeAResoudre->Xmin[Var] << ", " << ProblemeAResoudre->Xmax[Var];


            // if (TransportCost->IntercoGereeAvecDesCouts == OUI_ANTARES)
            {
                Var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[Interco];

                // if (TransportCost->IntercoGereeAvecLoopFlow == OUI_ANTARES)
                //     Xmax[Var] = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco]
                //                 - ValeursDeNTC->ValeurDeLoopFlowOrigineVersExtremite[Interco];
                // else
                //     Xmax[Var] = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco];

                Xmax[Var] = 3000; 
                Xmax[Var] += 0.01;
                ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
                if (Math::Infinite(Xmax[Var]) == 1)
                {
                    ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
                }

                Xmin[Var] = 0.0;
                // ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsReduits[Var] = NULL;
                // ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = NULL; // not adding AdresseDuResultat! // todo: remove

                logs.debug() << Var << ": " << ProblemeAResoudre->Xmin[Var] << ", " << ProblemeAResoudre->Xmax[Var];

                Var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[Interco];
                // if (TransportCost->IntercoGereeAvecLoopFlow == OUI_ANTARES)
                //     Xmax[Var] = ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]
                //                 + ValeursDeNTC->ValeurDeLoopFlowOrigineVersExtremite[Interco];
                // else
                //     Xmax[Var] = ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco];

                Xmax[Var] = 4000; 
                Xmax[Var] += 0.01;
                ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
                if (Math::Infinite(Xmax[Var]) == 1)
                {
                    ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
                }
                Xmin[Var] = 0.0;
                // ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsReduits[Var] = NULL;
                // ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = NULL;

                logs.debug() << Var << ": " << ProblemeAResoudre->Xmin[Var] << ", " << ProblemeAResoudre->Xmax[Var];

            }
        }
    }
    // todo: remove 
    // Loop flow: amount of power flowing circularly though the grid
    // when all "nodes" are perfectly balanced (no import and no
    // export). Such loop flows may be expected on any "simplified" grid
    // in which large regions (or even countries) are modeled by a small
    // number of "macro" nodes, and should accordingly be accounted for.
    // Flow circulating through the grid when all areas have a zero
    // import/export balance. This flow, to be put down to the
    // simplification of the real grid, is not subject to hurdle costs
    // in the course of the optimization
    // if (FLOW.LIN –LOOP FLOW) > 0
    // HURD. COST = (hourly direct hurdle cost) * (FLOW LIN.)
    // else HURD.COST = (hourly indirect hurdle cost) * (-1) * (FLOW LIN.)

    return;
}