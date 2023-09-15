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

#include "count_constraints_variables.h"
#include "../simulation/adequacy_patch_runtime_data.h"

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
            numberOfConstraints++;
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
            numberOfConstraints++;
    }
    // Special case of the hourly binding constraints
    const auto MatrixBindingConstraints = problemeHebdo->MatriceDesContraintesCouplantes;
    for (uint32_t CntCouplante = 0; CntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
         CntCouplante++)
    {
        if (MatrixBindingConstraints[CntCouplante].TypeDeContrainteCouplante == CONTRAINTE_HORAIRE)
            numberOfConstraints++;
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
            numberOfVariables += 2; // ENS, spilled energy
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
