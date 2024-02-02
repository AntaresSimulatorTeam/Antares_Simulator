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

#include "LinearProblemMatrix.h"
#include "opt_export_structure.h"
#include "antares/solver/utils/filename.h"
#include "opt_fonctions.h"
#include "LinearProblemMatrixStartUpCosts.h"

using namespace Antares::Data;

LinearProblemMatrix::LinearProblemMatrix(PROBLEME_HEBDO* problemeHebdo,
                                         ConstraintBuilder& builder) :
 ProblemMatrixEssential(problemeHebdo),
 builder_(builder),
 group1_(problemeHebdo, builder),
 bindingConstraintDayGroup_(problemeHebdo, builder),
 bindingConstraintWeekGroup_(problemeHebdo, builder),
 hydroPowerGroup_(problemeHebdo, builder),
 hydraulicSmoothingGroup_(problemeHebdo, builder),
 minMaxHydroPowerGroup_(problemeHebdo, builder),
 maxPumpingGroup_(problemeHebdo, builder),
 areaHydroLevelGroup_(problemeHebdo, builder),
 finalStockGroup_(problemeHebdo, builder)
{
    constraintgroups_ = {&group1_,
                         &bindingConstraintDayGroup_,
                         &bindingConstraintWeekGroup_,
                         &hydroPowerGroup_,
                         &hydraulicSmoothingGroup_,
                         &minMaxHydroPowerGroup_,
                         &maxPumpingGroup_,
                         &areaHydroLevelGroup_,
                         &finalStockGroup_};
}

void LinearProblemMatrix::Run()
{
    InitializeProblemAResoudreCounters();

    ProblemMatrixEssential::Run();

    if (problemeHebdo_->OptimisationAvecCoutsDeDemarrage)
    {
        LinearProblemMatrixStartUpCosts(problemeHebdo_, false, builder_).Run();
    }

    return;
}
