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

#include "pi_constantes_externes.h"

#include <math.h>
#include <yuni/core/math.h>

using namespace Yuni;

namespace
{
void setBoundsOnENS(const PROBLEME_HEBDO* ProblemeHebdo,
                    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre,
                    int hour)
{
    double* AdresseDuResultat;
    const CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour];
    double csrSolverRelaxation = ProblemeHebdo->adqPatchParams->ThresholdCSRVarBoundsRelaxation;

    // variables: ENS for each area inside adq patch
    for (int area = 0; area < ProblemeHebdo->NombreDePays; ++area)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            int Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[area];

            ProblemeAResoudre->Xmin[Var] = -csrSolverRelaxation;
            ProblemeAResoudre->Xmax[Var]
              = ProblemeHebdo->ResultatsHoraires[area]->ValeursHorairesDENS[hour]
                + csrSolverRelaxation;

            ProblemeAResoudre->X[Var]
              = ProblemeHebdo->ResultatsHoraires[area]->ValeursHorairesDeDefaillancePositive[hour];

            AdresseDuResultat = &(
              ProblemeHebdo->ResultatsHoraires[area]->ValeursHorairesDeDefaillancePositive[hour]);

            ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var]
              = AdresseDuResultat;

            logs.debug() << Var << ": " << ProblemeAResoudre->Xmin[Var] << ", "
                         << ProblemeAResoudre->Xmax[Var];
        }
    }
}

void setBoundsOnSpilledEnergy(const PROBLEME_HEBDO* ProblemeHebdo,
                              PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre,
                              int hour)
{
    const auto * CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour];
    double csrSolverRelaxation = ProblemeHebdo->adqPatchParams->ThresholdCSRVarBoundsRelaxation;

    // variables: Spilled Energy for each area inside adq patch
    for (int area = 0; area < ProblemeHebdo->NombreDePays; ++area)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            int Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[area];

            ProblemeAResoudre->Xmin[Var] = -csrSolverRelaxation;
            ProblemeAResoudre->Xmax[Var] = LINFINI_ANTARES;

            ProblemeAResoudre->X[Var]
              = ProblemeHebdo->ResultatsHoraires[area]->ValeursHorairesDeDefaillanceNegative[hour];

            double* AdresseDuResultat = &(
              ProblemeHebdo->ResultatsHoraires[area]->ValeursHorairesSpilledEnergyAfterCSR[hour]);

            ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var]
              = AdresseDuResultat;

            logs.debug() << Var << ": " << ProblemeAResoudre->Xmin[Var] << ", "
                         << ProblemeAResoudre->Xmax[Var];
        }
    }
}

void setBoundsOnFlows(const PROBLEME_HEBDO* ProblemeHebdo,
                      PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre,
                      int hour)
{
    double csrSolverRelaxation = ProblemeHebdo->adqPatchParams->ThresholdCSRVarBoundsRelaxation;
    const CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim
        = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour];
    double* Xmin = ProblemeAResoudre->Xmin;
    double* Xmax = ProblemeAResoudre->Xmax;
    VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeNTC = ProblemeHebdo->ValeursDeNTC[hour];

    // variables bounds: transmissin flows (flow, direct_direct and flow_indirect). For links
    // between nodes of type 2. Set hourly bounds for links between nodes of type 2, depending on
    // the user input (max direct and indirect flow).
    for (int Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; ++Interco)
    {
        // only consider link between 2 and 2
        if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaMode[Interco]
              != Data::AdequacyPatch::physicalAreaInsideAdqPatch
            || ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaMode[Interco]
                 != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            continue;
        }

        // flow
        int Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
        Xmax[Var]
            = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco] + csrSolverRelaxation;
        Xmin[Var]
            = -(ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]) - csrSolverRelaxation;
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

        double* AdresseDuResultat = &(ValeursDeNTC->ValeurDuFlux[Interco]);
        ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var]
            = AdresseDuResultat;

        logs.debug() << Var << ": " << ProblemeAResoudre->Xmin[Var] << ", "
            << ProblemeAResoudre->Xmax[Var];

        // direct / indirect flow
        Var = CorrespondanceVarNativesVarOptim
            ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[Interco];

        Xmin[Var] = -csrSolverRelaxation;
        Xmax[Var]
            = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco] + csrSolverRelaxation;
        ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
        if (Math::Infinite(Xmax[Var]) == 1)
        {
            ProblemeAResoudre->TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
        }

        logs.debug() << Var << ": " << ProblemeAResoudre->Xmin[Var] << ", "
            << ProblemeAResoudre->Xmax[Var];

        Var = CorrespondanceVarNativesVarOptim
            ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[Interco];

        Xmin[Var] = -csrSolverRelaxation;
        Xmax[Var]
            = ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco] + csrSolverRelaxation;
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
  const PROBLEME_HEBDO* ProblemeHebdo,
  PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre,
  int hour)
{
    logs.debug() << "[CSR] bounds";

    for (int Var = 0; Var < ProblemeAResoudre->NombreDeVariables; Var++)
        ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = nullptr;
    setBoundsOnENS(ProblemeHebdo, ProblemeAResoudre, hour);
    setBoundsOnSpilledEnergy(ProblemeHebdo, ProblemeAResoudre, hour);
    setBoundsOnFlows(ProblemeHebdo, ProblemeAResoudre, hour);
}
