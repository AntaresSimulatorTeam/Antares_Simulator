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
const double csrSolverRelaxation = 1e-3;

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

void setBoundsOnENS(PROBLEME_HEBDO* ProblemeHebdo, HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    int Var;
    double* AdresseDuResultat;
    int hour;
    hour = hourlyCsrProblem.hourInWeekTriggeredCsr;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour];

    // variables: ENS for each area inside adq patch
    for (int area = 0; area < ProblemeHebdo->NombreDePays; ++area)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[area];

            ProblemeAResoudre->Xmin[Var] = -csrSolverRelaxation;
            ProblemeAResoudre->Xmax[Var] = hourlyCsrProblem.densNewValues[area] + csrSolverRelaxation;

            ProblemeAResoudre->X[Var]
              = ProblemeHebdo->ResultatsHoraires[area]->ValeursHorairesDeDefaillancePositive[hour];

            AdresseDuResultat = &(
              ProblemeHebdo->ResultatsHoraires[area]->ValeursHorairesDeDefaillancePositive[hour]);

            ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var]
              = AdresseDuResultat;

            // update DENS values with densNew values
            ProblemeHebdo->ResultatsHoraires[area]->ValeursHorairesDENS[hour]
              = hourlyCsrProblem.densNewValues[area];

            logs.debug() << Var << ": " << ProblemeAResoudre->Xmin[Var] << ", "
                         << ProblemeAResoudre->Xmax[Var];
        }
    }
}

void setBoundsOnSpilledEnergy(PROBLEME_HEBDO* ProblemeHebdo, HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    int Var;
    double* AdresseDuResultat;
    int hour;
    hour = hourlyCsrProblem.hourInWeekTriggeredCsr;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour];

    // variables: Spilled Energy for each area inside adq patch 
    // todo after debugging transfer this into same area loop as ENS
    for (int area = 0; area < ProblemeHebdo->NombreDePays; ++area)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[area];

            ProblemeAResoudre->Xmin[Var] = -csrSolverRelaxation;
            ProblemeAResoudre->Xmax[Var] = LINFINI_ANTARES;

            ProblemeAResoudre->X[Var]
              = ProblemeHebdo->ResultatsHoraires[area]->ValeursHorairesDeDefaillanceNegative[hour];

            AdresseDuResultat = &(
              ProblemeHebdo->ResultatsHoraires[area]->ValeursHorairesDeDefaillanceNegative[hour]);

            ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var]
              = AdresseDuResultat;

            logs.debug() << Var << ": " << ProblemeAResoudre->Xmin[Var] << ", "
                         << ProblemeAResoudre->Xmax[Var];
        }
    }
}

void setBoundsOnFlows(PROBLEME_HEBDO* ProblemeHebdo, HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    int Var;
    double* AdresseDuResultat;
    int hour;
    hour = hourlyCsrProblem.hourInWeekTriggeredCsr;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour];
    int* TypeDeVariable;
    double* Xmin;
    double* Xmax;
    Xmin = ProblemeAResoudre->Xmin;
    Xmax = ProblemeAResoudre->Xmax;
    VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeNTC;
    ValeursDeNTC = ProblemeHebdo->ValeursDeNTC[hour];

    // variables bounds: transmissin flows (flow, direct_direct and flow_indirect). For links
    // between nodes of type 2. Set hourly bounds for links between nodes of type 2, depending on
    // the user input (max direct and indirect flow).
    for (int Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; ++Interco)
    {
        // only consider link between 2 and 2
        if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaMode[Interco]
              == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch
            && ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaMode[Interco]
                 == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            // flow
            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
            Xmax[Var] = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco] + csrSolverRelaxation;
            Xmin[Var] = -(ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]) - csrSolverRelaxation;
            ProblemeAResoudre->X[Var] = ValeursDeNTC->ValeurDuFlux[Interco];

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

            AdresseDuResultat = &(ValeursDeNTC->ValeurDuFlux[Interco]);
            ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var]
              = AdresseDuResultat;

            logs.debug() << Var << ": " << ProblemeAResoudre->Xmin[Var] << ", "
                         << ProblemeAResoudre->Xmax[Var];

            // direct / indirect flow
            Var = CorrespondanceVarNativesVarOptim
                    ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[Interco];
            // CSR Todo?
            // if (TransportCost->IntercoGereeAvecLoopFlow == OUI_ANTARES)
            //     Xmax[Var] = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco]
            //                 - ValeursDeNTC->ValeurDeLoopFlowOrigineVersExtremite[Interco];
            // else
            //     Xmax[Var] = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco];

            Xmin[Var] = -csrSolverRelaxation;
            Xmax[Var] = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco] + csrSolverRelaxation;
            ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
            if (Math::Infinite(Xmax[Var]) == 1)
            {
                ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
            }

            logs.debug() << Var << ": " << ProblemeAResoudre->Xmin[Var] << ", "
                         << ProblemeAResoudre->Xmax[Var];

            Var = CorrespondanceVarNativesVarOptim
                    ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[Interco];
            // CSR Todo?
            // if (TransportCost->IntercoGereeAvecLoopFlow == OUI_ANTARES)
            //     Xmax[Var] = ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]
            //                 + ValeursDeNTC->ValeurDeLoopFlowOrigineVersExtremite[Interco];
            // else
            //     Xmax[Var] = ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco];

            Xmin[Var] = -csrSolverRelaxation;
            Xmax[Var] = ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco] + csrSolverRelaxation;
            ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
            if (Math::Infinite(Xmax[Var]) == 1)
            {
                ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
            }

            logs.debug() << Var << ": " << ProblemeAResoudre->Xmin[Var] << ", "
                         << ProblemeAResoudre->Xmax[Var];
        }
    }
}

void OPT_InitialiserLesBornesDesVariablesDuProblemeQuadratique_CSR(
  PROBLEME_HEBDO* ProblemeHebdo,
  HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    logs.debug() << "[CSR] bounds";

    int Var;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;

    for (Var = 0; Var < ProblemeAResoudre->NombreDeVariables; Var++)
        ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = NULL;

    setBoundsOnENS(ProblemeHebdo, hourlyCsrProblem);
    setBoundsOnSpilledEnergy(ProblemeHebdo, hourlyCsrProblem);
    setBoundsOnFlows(ProblemeHebdo, hourlyCsrProblem);
}