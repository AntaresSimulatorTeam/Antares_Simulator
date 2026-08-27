// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LinearProblemMatrixRamping.h"
#include "antares/solver/optimisation/constraints/ConstraintBuilder.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/simulation/simulation.h"

void OPT_DecompteDesVariablesEtDesContraintesRampesThermiques(PROBLEME_HEBDO* problemeHebdo)
{
    ConstraintBuilder builder(problemeHebdo);
    LinearProblemMatrixRamping(problemeHebdo, true, builder).Run();

    OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaireRampesThermiques(problemeHebdo,
                                                                                  true);
}
