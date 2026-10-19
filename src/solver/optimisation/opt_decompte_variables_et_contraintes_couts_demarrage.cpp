// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LinearProblemMatrixStartUpCosts.h"
#include "antares/solver/optimisation/constraints/ConstraintBuilder.h"

void OPT_DecompteDesVariablesEtDesContraintesCoutsDeDemarrage(PROBLEME_HEBDO* problemeHebdo)
{
    if (!problemeHebdo->OptimisationNotFastMode)
    {
        return;
    }

    ConstraintBuilder builder(problemeHebdo);
    LinearProblemMatrixStartUpCosts(problemeHebdo, true, builder).Run();

    OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaireCoutsDeDemarrage(problemeHebdo,
                                                                                  true);
}
