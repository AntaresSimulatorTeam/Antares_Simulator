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
// Republishes the post-processed results into the solution vectors for the
// duration of a fill, and puts the solver state back exactly as it was.
class SolutionRefreshedFromResults
{
public:
    explicit SolutionRefreshedFromResults(PROBLEME_ANTARES_A_RESOUDRE& problem):
        problem_(problem),
        savedX_(problem.X),
        savedDuals_(problem.CoutsMarginauxDesContraintes)
    {
        refresh(problem_.X, problem_.AdresseOuPlacerLaValeurDesVariablesOptimisees);
        refresh(problem_.CoutsMarginauxDesContraintes,
                problem_.AdresseOuPlacerLaValeurDesCoutsMarginaux);
    }

    ~SolutionRefreshedFromResults()
    {
        problem_.X = std::move(savedX_);
        problem_.CoutsMarginauxDesContraintes = std::move(savedDuals_);
    }

    SolutionRefreshedFromResults(const SolutionRefreshedFromResults&) = delete;
    SolutionRefreshedFromResults& operator=(const SolutionRefreshedFromResults&) = delete;

private:
    // Entries without an address are never published to the results, so they
    // keep the value the optimizer left -- which is what post-processing could
    // not have changed anyway.
    static void refresh(std::vector<double>& values, const std::vector<double*>& addresses)
    {
        const std::size_t count = std::min(values.size(), addresses.size());
        for (std::size_t i = 0; i < count; ++i)
        {
            if (addresses[i] != nullptr)
            {
                values[i] = *addresses[i];
            }
        }
    }

    PROBLEME_ANTARES_A_RESOUDRE& problem_;
    std::vector<double> savedX_;
    std::vector<double> savedDuals_;
};

std::once_flag dailyRangeWarningFlag;
} // namespace

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

    const SolutionRefreshedFromResults refreshed(*problemeHebdo.ProblemeAResoudre);
    FillLegacySimulationTable(simulationTable,
                              problemeHebdo,
                              fillContext,
                              nameMapper,
                              currentBlock);
}

} // namespace Antares::Optimization
