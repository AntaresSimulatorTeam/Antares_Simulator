// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/opt_decompte_variables_et_contraintes_reserves.h"

#include "antares/solver/optimisation/LinearProblemMatrixReserves.h"
#include "antares/solver/optimisation/constraints/constraint_builder_utils.h"
#include "antares/solver/optimisation/opt_construction_variables_reserves.h"
#include "antares/solver/simulation/simulation.h"

void OPT_DecompteDesVariablesEtDesContraintesReserves(PROBLEME_HEBDO* problemeHebdo)
{
    auto builder_data = NewGetConstraintBuilderFromProblemHebdo(problemeHebdo);
    ConstraintBuilder builder(builder_data);
    LinearProblemMatrixReserves(problemeHebdo, true, builder).Run();

    OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaireReserves(problemeHebdo, true);
}
