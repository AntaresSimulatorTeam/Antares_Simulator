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

void constructVariableENS(PROBLEME_HEBDO* ProblemeHebdo, HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    int hour = hourlyCsrProblem.hourInWeekTriggeredCsr;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    int& NumberOfVariables = ProblemeAResoudre->NombreDeVariables;
    NumberOfVariables = 0;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour];

    // variables: ENS of each area inside adq patch
    logs.debug() << " ENS of each area inside adq patch: ";
    for (int area = 0; area < ProblemeHebdo->NombreDePays; ++area)
    {
        // Only ENS for areas inside adq patch are considered as variables
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[area]
              = NumberOfVariables;
            ProblemeAResoudre->TypeDeVariable[NumberOfVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
            hourlyCsrProblem.varToBeSetToZeroIfBelowThreshold.insert(NumberOfVariables);
            hourlyCsrProblem.ensSet.insert(NumberOfVariables);
            logs.debug() << NumberOfVariables << " ENS[" << area << "].-["
                         << ProblemeHebdo->NomsDesPays[area] << "].";

            NumberOfVariables++;
        }
    }
}

void constructVariableSpilledEnergy(PROBLEME_HEBDO* ProblemeHebdo,
                                    HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    int hour = hourlyCsrProblem.hourInWeekTriggeredCsr;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour];
    int& NumberOfVariables = ProblemeAResoudre->NombreDeVariables;

    // variables: Spilled Energy  of each area inside adq patch
    logs.debug() << " Spilled Energy  of each area inside adq patch: ";
    for (int area = 0; area < ProblemeHebdo->NombreDePays; ++area)
    {
        // Only Spilled Energy  for areas inside adq patch are considered as variables
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[area]
              = NumberOfVariables;
            ProblemeAResoudre->TypeDeVariable[NumberOfVariables] = VARIABLE_BORNEE_INFERIEUREMENT;
            hourlyCsrProblem.varToBeSetToZeroIfBelowThreshold.insert(NumberOfVariables);
            logs.debug() << NumberOfVariables << " Spilled Energy[" << area << "].-["
                         << ProblemeHebdo->NomsDesPays[area] << "].";

            NumberOfVariables++;
        }
    }
}

void constructVariableFlows(PROBLEME_HEBDO* ProblemeHebdo, HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    int hour = hourlyCsrProblem.hourInWeekTriggeredCsr;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[hour];
    int& NumberOfVariables = ProblemeAResoudre->NombreDeVariables;

    // variables: transmissin flows (flow, direct_direct and flow_indirect). For links between 2
    // and 2.
    logs.debug()
      << " transmissin flows (flow, flow_direct and flow_indirect). For links between 2 and 2:";
    for (int Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; Interco++)
    {
        // only consider link between 2 and 2
        if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaMode[Interco]
              == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch
            && ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaMode[Interco]
                 == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco]
              = NumberOfVariables;
            ProblemeAResoudre->TypeDeVariable[NumberOfVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
            hourlyCsrProblem.linkSet.insert(NumberOfVariables);
            logs.debug()
              << NumberOfVariables << " flow[" << Interco << "]. ["
              << ProblemeHebdo->NomsDesPays[ProblemeHebdo->PaysExtremiteDeLInterconnexion[Interco]]
              << "]-["
              << ProblemeHebdo->NomsDesPays[ProblemeHebdo->PaysOrigineDeLInterconnexion[Interco]]
              << "].";
            NumberOfVariables++;

            CorrespondanceVarNativesVarOptim
              ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[Interco]
              = NumberOfVariables;
            ProblemeAResoudre->TypeDeVariable[NumberOfVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
            logs.debug() << NumberOfVariables << " direct flow[" << Interco << "]. ";
            NumberOfVariables++;

            CorrespondanceVarNativesVarOptim
              ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[Interco]
              = NumberOfVariables;
            ProblemeAResoudre->TypeDeVariable[NumberOfVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
            logs.debug() << NumberOfVariables << " indirect flow[" << Interco << "]. ";
            NumberOfVariables++;
        }
    }
}

void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeQuadratique_CSR(
  PROBLEME_HEBDO* ProblemeHebdo,
  HOURLY_CSR_PROBLEM& hourlyCsrProblem)
{
    logs.debug() << "[CSR] variable list:";

    const PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
    assert(ProblemeAResoudre != NULL);

    constructVariableENS(ProblemeHebdo, hourlyCsrProblem);
    constructVariableSpilledEnergy(ProblemeHebdo, hourlyCsrProblem);
    constructVariableFlows(ProblemeHebdo, hourlyCsrProblem);

    return;
}
