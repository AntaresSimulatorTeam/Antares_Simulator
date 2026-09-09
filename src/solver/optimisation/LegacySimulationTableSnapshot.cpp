// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LegacySimulationTableSnapshot.h"

#include <algorithm>
#include <cassert>
#include <mutex>
#include <utility>
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
// The post-processed solution, read back from the result address tables without
// touching the solver state. Post-processes mutate PROBLEME_HEBDO's result
// structures in place and never write into ProblemeAResoudre's X / duals, and
// each address points at the exact result slot the solve published with
// `*address = X[i]`.
struct StageSolution
{
    std::vector<double> primal;
    std::vector<double> duals;
};

// Entries without an address are never published to the results, so they keep
// the value the optimizer left -- which is what post-processing could not have
// changed anyway.
std::vector<double> readBackFromAddresses(const std::vector<double>& values,
                                          const std::vector<double*>& addresses)
{
    std::vector<double> out = values;
    const std::size_t count = std::min(out.size(), addresses.size());
    for (std::size_t i = 0; i < count; ++i)
    {
        if (addresses[i] != nullptr)
        {
            out[i] = *addresses[i];
        }
    }
    return out;
}

StageSolution stageSolution(const PROBLEME_ANTARES_A_RESOUDRE& problem)
{
    return {readBackFromAddresses(problem.X, problem.AdresseOuPlacerLaValeurDesVariablesOptimisees),
            readBackFromAddresses(problem.CoutsMarginauxDesContraintes,
                                  problem.AdresseOuPlacerLaValeurDesCoutsMarginaux)};
}

std::once_flag dailyRangeWarningFlag;
} // namespace

void FillLegacySimulationTable(SimulationTable& simulationTable,
                               PROBLEME_HEBDO& problemeHebdo,
                               const LegacySolution& solution,
                               const FillContext& fillContext,
                               const LegacyNameMapper& nameMapper,
                               unsigned currentBlock,
                               const InactiveComponentsAnalyzer* inactiveComponents)
{
    const PROBLEME_ANTARES_A_RESOUDRE& problem = *problemeHebdo.ProblemeAResoudre;

    // LegacyVariablesInfo and CoutLineaire are sized to NombreDeVariables in
    // resizeProbleme, and the solution view mirrors X, so the index-based reads
    // below are always in bounds.
    assert(problem.LegacyVariablesInfo.size() == static_cast<std::size_t>(problem.NombreDeVariables)
           && solution.primal.size() == static_cast<std::size_t>(problem.NombreDeVariables));
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
           .value = solution.primal[static_cast<std::size_t>(index)],
           .status = std::nullopt});
    }

    AddLegacyExtraOutputs(simulationTable,
                          problemeHebdo,
                          solution,
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
                                         unsigned currentBlock,
                                         const InactiveComponentsAnalyzer* inactiveComponents)
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

    const StageSolution solution = stageSolution(*problemeHebdo.ProblemeAResoudre);
    FillLegacySimulationTable(simulationTable,
                              problemeHebdo,
                              {solution.primal, solution.duals},
                              fillContext,
                              nameMapper,
                              currentBlock,
                              inactiveComponents);
}

} // namespace Antares::Optimization
