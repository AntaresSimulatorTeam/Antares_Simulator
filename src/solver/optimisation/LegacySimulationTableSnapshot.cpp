// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LegacySimulationTableSnapshot.h"

#include <cassert>

#include "antares/solver/optimisation/LegacyExtraOutputs.h"
#include "antares/solver/optimisation/LegacyVariableInfo.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using Antares::IO::Outputs::SimulationTable;
using Antares::LinearProblem::Api::FillContext;

namespace Antares::Optimization
{

void FillLegacySimulationTable(SimulationTable& simulationTable,
                               PROBLEME_HEBDO& problemeHebdo,
                               const FillContext& fillContext,
                               const LegacyNameMapper& nameMapper,
                               unsigned currentBlock)
{
    const PROBLEME_ANTARES_A_RESOUDRE& problem = *problemeHebdo.ProblemeAResoudre;

    // LegacyVariablesInfo, X and CoutLineaire are all sized to NombreDeVariables
    // in resizeProbleme, so the index-based reads below are always in bounds.
    assert(problem.LegacyVariablesInfo.size() == static_cast<std::size_t>(problem.NombreDeVariables)
           && problem.X.size() == static_cast<std::size_t>(problem.NombreDeVariables)
           && problem.CoutLineaire.size() == static_cast<std::size_t>(problem.NombreDeVariables));
    for (int index = 0; index < problem.NombreDeVariables; ++index)
    {
        const auto& info = problem.LegacyVariablesInfo[static_cast<std::size_t>(index)];
        if (!info)
        {
            continue;
        }

        simulationTable.addEntry(
          {.block = currentBlock,
           .component = info->component,
           .output = nameMapper.mapOutput(info->name),
           .absolute_time_index = info->timeIndex,
           .block_time_index = LegacyBlockTimeIndex(fillContext, info->timeIndex),
           .scenario_index = fillContext.getYear(),
           .value = problem.X[static_cast<std::size_t>(index)],
           .status = std::nullopt});
    }

    AddLegacyExtraOutputs(simulationTable, problemeHebdo, fillContext, currentBlock);
}

} // namespace Antares::Optimization
