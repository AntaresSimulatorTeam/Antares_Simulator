// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LegacySimulationTableSnapshot.h"

#include <algorithm>
#include <cassert>
#include <mutex>
#include <vector>

#include <antares/antares/constants.h>
#include <antares/logs/logs.h>
#include "antares/io/outputs/SimulationTableGenerator.h"
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/solver/modeler/ModelerData.h"
#include "antares/solver/optimisation/LegacyExtraOutputs.h"
#include "antares/solver/optimisation/LegacyVariableInfo.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using Antares::IO::Outputs::SimulationTable;
using Antares::LinearProblem::Api::FillContext;

namespace Antares::Optimization
{

namespace
{
// Rebuilds the solution vector as it stands *after* post-processing.
//
// Post-processing mutates the result structures in place (the slots the address
// table points at) and never writes back into the solver's X / duals, so a copy
// straight from the solver would carry the pre-post-process values. Reading those
// addresses back into a scratch copy of the solver values recovers the
// post-processed state. Entries without an address were never published to the
// results, so they keep the value the optimizer left.
std::vector<double> GatherFromAddresses(const std::vector<double>& solverValues,
                                        const std::vector<double*>& addresses)
{
    std::vector<double> values(solverValues);
    const std::size_t count = std::min(values.size(), addresses.size());
    for (std::size_t i = 0; i < count; ++i)
    {
        if (addresses[i] != nullptr)
        {
            values[i] = *addresses[i];
        }
    }
    return values;
}

std::once_flag dailyRangeWarningFlag;
} // namespace

void FillLegacySimulationTable(SimulationTable& simulationTable,
                               PROBLEME_HEBDO& problemeHebdo,
                               const std::vector<double>& x,
                               const std::vector<double>& coutsMarginaux,
                               const FillContext& fillContext,
                               const LegacyNameMapper& nameMapper,
                               unsigned currentBlock,
                               const InactiveComponentsAnalyzer* inactiveComponents)
{
    const PROBLEME_ANTARES_A_RESOUDRE& problem = *problemeHebdo.ProblemeAResoudre;

    // LegacyVariablesInfo and x are all sized to NombreDeVariables in
    // resizeProbleme, so the index-based reads below are always in bounds.
    assert(problem.LegacyVariablesInfo.size() == static_cast<std::size_t>(problem.NombreDeVariables)
           && x.size() == static_cast<std::size_t>(problem.NombreDeVariables));
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
           .value = x[static_cast<std::size_t>(index)],
           .status = std::nullopt});
    }

    AddLegacyExtraOutputs(simulationTable,
                          problemeHebdo,
                          x,
                          coutsMarginaux,
                          fillContext,
                          currentBlock,
                          inactiveComponents);
}

unsigned LegacyWeeklyBlock(const PROBLEME_HEBDO& problemeHebdo)
{
    return static_cast<unsigned>(problemeHebdo.HeureDansLAnnee) / Constants::nbHoursInAWeek;
}

void DumpSimulationTableAfterPostProcess(SimulationTable& simulationTable,
                                         PROBLEME_HEBDO& problemeHebdo,
                                         const FillContext& fillContext,
                                         unsigned currentBlock)
{
    if (!problemeHebdo.OptimisationAuPasHebdomadaire)
    {
        std::call_once(dailyRangeWarningFlag,
                       []
                       {
                           logs.warning() << "Simulation tables are not produced after "
                                             "post-processing when the simplex optimization "
                                             "range is daily";
                       });
        return;
    }

    // Modeler rows first, as during the solve, so a stage table has the same
    // row order as the optimisation ones.
    if (const auto& solved = problemeHebdo.lastSolvedModelerProblem;
        solved && problemeHebdo.modelerData)
    {
        IO::Outputs::FillSimulationTable(simulationTable,
                                         *solved->problem,
                                         solved->objectiveValue,
                                         *problemeHebdo.modelerData,
                                         *solved->entities,
                                         fillContext,
                                         currentBlock,
                                         IO::Outputs::TimeConversionMode::WeeklyBlocks,
                                         true);
    }

    static constexpr LegacyNameMapper nameMapper;

    // Post-processing moved the results but not the solver's X / duals, so
    // rebuild both from the address table to republish the post-processed state.
    const PROBLEME_ANTARES_A_RESOUDRE& problem = *problemeHebdo.ProblemeAResoudre;
    const std::vector<double> x = GatherFromAddresses(
      problem.X,
      problem.AdresseOuPlacerLaValeurDesVariablesOptimisees);
    const std::vector<double> coutsMarginaux = GatherFromAddresses(
      problem.CoutsMarginauxDesContraintes,
      problem.AdresseOuPlacerLaValeurDesCoutsMarginaux);

    FillLegacySimulationTable(simulationTable,
                              problemeHebdo,
                              x,
                              coutsMarginaux,
                              fillContext,
                              nameMapper,
                              currentBlock);
}

} // namespace Antares::Optimization
