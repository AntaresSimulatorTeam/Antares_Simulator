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

void OPT_InitialiserLesBornesDesVariablesDuProblemeQuadratique_CSR(
  PROBLEME_HEBDO* ProblemeHebdo,
  HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    // CSR todo initialize uppper bound and lower bound of variables in hourly CSR quadratic
    // CSR todo: let us first to create an optim problem like this:
    //  variables: ENS of each area
    //  objective function: Sum (2 * (ENS)^2) of all area
    //  upper bound and lower bound: for each ENS: 100 <= ENS <= 3000
    //  constraint: No constraint
    // CSR todo, we re-use ProblemeAResoudre from weekly ProblemeHebdo, shall we instead use a new
    // one created inside HOURLY_CSR_PROBLEM?

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
    // CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[0]; //CSR todo; this should be 0 not hour ???

    for (int area = 0; area < ProblemeHebdo->NombreDePays; ++area)
    {
        Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[area];

        ProblemeAResoudre->Xmin[Var] = 100;
        ProblemeAResoudre->Xmax[Var] = 3000;

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
        AdresseDuResultat
          = &(ProblemeHebdo->ResultatsHoraires[area]->ValeursHorairesDeDefaillancePositive[hour]);

        ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = AdresseDuResultat;
    }

    return;
}