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

void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeQuadratique(PROBLEME_HEBDO* ProblemeHebdo)
{
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    int Interco;
    int NombreDeVariables = 0;

    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;

    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    assert(ProblemeAResoudre != NULL);

    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[0];

    for (Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; Interco++)
    {
        CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco]
          = NombreDeVariables;
        ProblemeAResoudre->TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
        NombreDeVariables++;
    }
    ProblemeAResoudre->NombreDeVariables = NombreDeVariables;
}

void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeQuadratique_CSR(PROBLEME_HEBDO* ProblemeHebdo, HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
  //CSR todo :build list of variable to be optimized in hourly CSR quadratic problem.

  //CSR todo: let us first to create an optim problem like this:
  // variables: ENS of each area
  // objective function: Sum (2 * (ENS)^2) of all area for for areas inside adq patch 
  // upper bound and lower bound: for each ENS: 100 <= ENS <= 3000
  // constraint: 2 * ENS < 1000

  PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
  int NumberOfVariables = 0;
  int hour = hourlyCsrProblem.hourInWeekTriggeredCsr;
  CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
  
  ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre; 
  assert(ProblemeAResoudre != NULL);

  CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour];

  for (int area = 0; area < ProblemeHebdo->NombreDePays; ++area)
  {
    //!!! Only ENS for areas inside adq patch are considered as variables
    if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[area] == Data::AdequacyPatch::adqmPhysicalAreaInsideAdqPatch)
    {
      CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[area] = NumberOfVariables;
      ProblemeAResoudre->TypeDeVariable[NumberOfVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
      NumberOfVariables++;
    }
  }
  ProblemeAResoudre->NombreDeVariables = NumberOfVariables;  
  return;
}