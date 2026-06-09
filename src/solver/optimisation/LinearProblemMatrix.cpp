// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LinearProblemMatrix.h"

#include "antares/solver/optimisation/LinearProblemMatrixReserves.h"
#include "antares/solver/optimisation/LinearProblemMatrixStartUpCosts.h"
#include "antares/solver/optimisation/opt_export_structure.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/utils/filename.h"

using namespace Antares::Data;

LinearProblemMatrix::LinearProblemMatrix(PROBLEME_HEBDO* problemeHebdo, ConstraintBuilder& builder):
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

    if (problemeHebdo_->OptimisationNotFastMode)
    {
        LinearProblemMatrixStartUpCosts(problemeHebdo_, false, builder_).Run();
        if (problemeHebdo_->allReserves)
        {
            LinearProblemMatrixReserves(problemeHebdo_, false, builder_).Run();
        }
    }

    return;
}
