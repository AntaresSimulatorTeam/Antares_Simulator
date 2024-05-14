/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/solver/optimisation/adequacy_patch_csr/hourly_csr_problem.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "pi_constantes_externes.h"

void HourlyCSRProblem::constructVariableENS()
{
    int& NumberOfVariables = problemeAResoudre_.NombreDeVariables;
    NumberOfVariables = 0;

    // variables: ENS of each area inside adq patch
    logs.debug() << " ENS of each area inside adq patch: ";
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        // Only ENS for areas inside adq patch are considered as variables
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            variableManager_.PositiveUnsuppliedEnergy(area, triggeredHour) = NumberOfVariables;
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
    int& NumberOfVariables = problemeAResoudre_.NombreDeVariables;

    // variables: Spilled Energy  of each area inside adq patch
    logs.debug() << " Spilled Energy  of each area inside adq patch: ";
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        // Only Spilled Energy  for areas inside adq patch are considered as variables
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            variableManager_.NegativeUnsuppliedEnergy(area, triggeredHour) = NumberOfVariables;
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
    int& NumberOfVariables = problemeAResoudre_.NombreDeVariables;

    // variables: transmissin flows (flow, direct_direct and flow_indirect). For links between 2
    // and 2.
    logs.debug()
      << " transmissin flows (flow, flow_direct and flow_indirect). For links between 2 and 2:";
    for (uint32_t Interco = 0; Interco < problemeHebdo_->NombreDInterconnexions; Interco++)
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
            algebraicFluxVar = variableManager_.NTCDirect(Interco, triggeredHour)
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

            directVar = variableManager_.IntercoDirectCost(Interco, triggeredHour)
              = NumberOfVariables;
            problemeAResoudre_.TypeDeVariable[NumberOfVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
            logs.debug() << NumberOfVariables << " direct flow[" << Interco << "]. ";
            NumberOfVariables++;

            indirectVar = variableManager_.IntercoIndirectCost(Interco, triggeredHour)
              = NumberOfVariables;
            problemeAResoudre_.TypeDeVariable[NumberOfVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
            logs.debug() << NumberOfVariables << " indirect flow[" << Interco << "]. ";
            NumberOfVariables++;

            linkInsideAdqPatch[algebraicFluxVar] = LinkVariable(directVar, indirectVar);
        }
    }
}
