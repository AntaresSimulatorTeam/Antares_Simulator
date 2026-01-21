// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/daily/h2o_j_donnees_mensuelles.h"
#include "antares/solver/hydro/daily/h2o_j_fonctions.h"

namespace DoneesOptimisationJournaliere
{
void H2O_J_ResoudreLeProblemeLineaire(DONNEES_MENSUELLES* DonneesMensuelles, int NumeroDeProbleme)
{
    // Delegate the actual optimisation work to the OR-Tools based
    // MPSolver helper. This keeps the public API unchanged while
    // moving the implementation away from the legacy SPX layer.
    H2O_J_MPSolver_Solve(DonneesMensuelles, NumeroDeProbleme);
}
} // namespace DoneesOptimisationJournaliere
