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
#include "../simulation/adequacy_patch_runtime_data.h"
#include "../hourly_csr_problem.h"

#include "pi_constantes_externes.h"
void HourlyCSRProblem::constructVariableENS()
{
    int& NumberOfVariables = problemeAResoudre_.NombreDeVariables;
    NumberOfVariables = 0;
    auto CorrespondanceVarNativesVarOptim
      = problemeHebdo_->CorrespondanceVarNativesVarOptim[triggeredHour];

    // variables: ENS of each area inside adq patch
    logs.debug() << " ENS of each area inside adq patch: ";
    for (int area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        // Only ENS for areas inside adq patch are considered as variables
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[area]
              = NumberOfVariables;
            problemeAResoudre_.TypeDeVariable[NumberOfVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
            varToBeSetToZeroIfBelowThreshold.insert(NumberOfVariables);
            ensVariablesInsideAdqPatch.insert(NumberOfVariables);
            logs.debug() << NumberOfVariables << " ENS[" << area << "].-["
                         << problemeHebdo_->NomsDesPays[area] << "].";

            NumberOfVariables++;
        }
    }
}

void HourlyCSRProblem::constructVariableSpilledEnergy()
{
    auto CorrespondanceVarNativesVarOptim
      = problemeHebdo_->CorrespondanceVarNativesVarOptim[triggeredHour];
    int& NumberOfVariables = problemeAResoudre_.NombreDeVariables;

    // variables: Spilled Energy  of each area inside adq patch
    logs.debug() << " Spilled Energy  of each area inside adq patch: ";
    for (int area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        // Only Spilled Energy  for areas inside adq patch are considered as variables
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[area]
              = NumberOfVariables;
            problemeAResoudre_.TypeDeVariable[NumberOfVariables] = VARIABLE_BORNEE_INFERIEUREMENT;
            varToBeSetToZeroIfBelowThreshold.insert(NumberOfVariables);
            logs.debug() << NumberOfVariables << " Spilled Energy[" << area << "].-["
                         << problemeHebdo_->NomsDesPays[area] << "].";

            NumberOfVariables++;
        }
    }
}

void HourlyCSRProblem::constructVariableFlows()
{
    auto CorrespondanceVarNativesVarOptim
      = problemeHebdo_->CorrespondanceVarNativesVarOptim[triggeredHour];
    int& NumberOfVariables = problemeAResoudre_.NombreDeVariables;

    // variables: transmissin flows (flow, direct_direct and flow_indirect). For links between 2
    // and 2.
    logs.debug()
      << " transmissin flows (flow, flow_direct and flow_indirect). For links between 2 and 2:";
    for (int Interco = 0; Interco < problemeHebdo_->NombreDInterconnexions; Interco++)
    {
        // only consider link between 2 and 2
        if (problemeHebdo_->adequacyPatchRuntimeData->originAreaMode[Interco]
              == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch
            && problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode[Interco]
                 == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            int algebraicFluxVar;
            int directVar;
            int indirectVar;
            algebraicFluxVar
              = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco]
              = NumberOfVariables;
            problemeAResoudre_.TypeDeVariable[NumberOfVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
            logs.debug()
              << NumberOfVariables << " flow[" << Interco << "]. ["
              << problemeHebdo_
                   ->NomsDesPays[problemeHebdo_->PaysExtremiteDeLInterconnexion[Interco]]
              << "]-["
              << problemeHebdo_->NomsDesPays[problemeHebdo_->PaysOrigineDeLInterconnexion[Interco]]
              << "].";
            NumberOfVariables++;

            directVar = CorrespondanceVarNativesVarOptim
                          ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[Interco]
              = NumberOfVariables;
            problemeAResoudre_.TypeDeVariable[NumberOfVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
            logs.debug() << NumberOfVariables << " direct flow[" << Interco << "]. ";
            NumberOfVariables++;

            indirectVar = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[Interco]
              = NumberOfVariables;
            problemeAResoudre_.TypeDeVariable[NumberOfVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
            logs.debug() << NumberOfVariables << " indirect flow[" << Interco << "]. ";
            NumberOfVariables++;

            linkInsideAdqPatch[algebraicFluxVar] = LinkVariable(directVar, indirectVar);
        }
    }
}
