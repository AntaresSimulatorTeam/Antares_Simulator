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

#include "antares/solver/optimisation/adequacy_patch_csr/count_constraints_variables.h"

#include "antares/solver/simulation/adequacy_patch_runtime_data.h"

namespace Antares::Data::AdequacyPatch
{
int countConstraints(const PROBLEME_HEBDO* problemeHebdo)
{
    int numberOfConstraints = 0;
    // constraint: Flow = Flow_direct - Flow_indirect (+ loop flow) for links between nodes of
    // type 2.
    for (uint32_t Interco = 0; Interco < problemeHebdo->NombreDInterconnexions; Interco++)
    {
        if (problemeHebdo->adequacyPatchRuntimeData->originAreaMode[Interco]
              == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch
            && problemeHebdo->adequacyPatchRuntimeData->extremityAreaMode[Interco]
                 == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            numberOfConstraints++;
        }
    }

    // constraint:
    // ENS(node A) +
    // - flow (A -> 2) or (+ flow (2 -> A)) there should be only one of them, otherwise double-count
    // - spillage(node A) =
    // ENS_init(node A) + net_position_init(node A) – spillage_init(node A)
    // for all areas inside adequacy patch
    for (uint32_t Area = 0; Area < problemeHebdo->NombreDePays; ++Area)
    {
        if (problemeHebdo->adequacyPatchRuntimeData->areaMode[Area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            numberOfConstraints++;
        }
    }
    // Special case of the hourly binding constraints
    const auto MatrixBindingConstraints = problemeHebdo->MatriceDesContraintesCouplantes;
    for (uint32_t CntCouplante = 0; CntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
         CntCouplante++)
    {
        if (MatrixBindingConstraints[CntCouplante].TypeDeContrainteCouplante == CONTRAINTE_HORAIRE)
        {
            numberOfConstraints++;
        }
    }
    return numberOfConstraints;
}

int countVariables(const PROBLEME_HEBDO* problemeHebdo)
{
    int numberOfVariables = 0;
    // variables: ENS of each area inside adq patch
    for (uint32_t area = 0; area < problemeHebdo->NombreDePays; ++area)
    {
        // Only ENS for areas inside adq patch are considered as variables
        if (problemeHebdo->adequacyPatchRuntimeData->areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            numberOfVariables += 2; // ENS, spilled energy
        }
    }

    for (uint32_t Interco = 0; Interco < problemeHebdo->NombreDInterconnexions; Interco++)
    {
        // only consider link between 2 and 2
        if (problemeHebdo->adequacyPatchRuntimeData->originAreaMode[Interco]
              == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch
            && problemeHebdo->adequacyPatchRuntimeData->extremityAreaMode[Interco]
                 == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            numberOfVariables += 3; // algebraic flow, direct flow, indirect flow
        }
    }
    return numberOfVariables;
}
} // namespace Antares::Data::AdequacyPatch
