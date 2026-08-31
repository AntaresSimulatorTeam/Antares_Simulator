// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LegacySimulationTableSnapshot.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::Optimization
{

void DumpSimulationTableStage(IO::Outputs::OptimisationsSimulationTable* tables,
                              const std::string& stage,
                              PROBLEME_HEBDO& problemeHebdo)
{
    if (tables == nullptr)
    {
        return;
    }

    // The whole week is one block here: post-processing runs once the week is
    // solved, not once per optimisation interval, hence NumIntervalle = 0.
    const auto fillContext = buildFillContext(&problemeHebdo, 0);
    DumpSimulationTableAfterPostProcess(*tables->tableForStage(stage),
                                        problemeHebdo,
                                        fillContext,
                                        LegacyWeeklyBlock(problemeHebdo));
}

} // namespace Antares::Optimization
