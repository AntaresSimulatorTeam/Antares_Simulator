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

#include "antares/solver/optimisation/constraints/BindingConstraintDayGroup.h"

#include "antares/solver/optimisation/constraints/constraint_builder_utils.h"

BindingConstraintDayData BindingConstraintDayGroup::GetBindingConstraintDayDataFromProblemHebdo()
{
    return {.MatriceDesContraintesCouplantes = problemeHebdo_->MatriceDesContraintesCouplantes,
            .PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays,
            .CorrespondanceCntNativesCntOptimJournalieres
            = problemeHebdo_->CorrespondanceCntNativesCntOptimJournalieres,
            .NombreDePasDeTempsDUneJournee = problemeHebdo_->NombreDePasDeTempsDUneJournee,
            .NumeroDeJourDuPasDeTemps = problemeHebdo_->NumeroDeJourDuPasDeTemps};
}

void BindingConstraintDayGroup::BuildConstraints()
{
    auto bindingConstraintDayData = GetBindingConstraintDayDataFromProblemHebdo();
    BindingConstraintDay bindingConstraintDay(builder_, bindingConstraintDayData);
    for (uint32_t cntCouplante = 0; cntCouplante < problemeHebdo_->NombreDeContraintesCouplantes;
         cntCouplante++)
    {
        bindingConstraintDay.add(cntCouplante);
    }
}
