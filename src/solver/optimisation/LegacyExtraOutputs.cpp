// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LegacyExtraOutputs.h"

#include <cmath>

#include "antares/solver/optimisation/LegacySolutionView.h"

using Antares::IO::Outputs::SimulationTable;
using Antares::Optimisation::LinearProblemApi::FillContext;

namespace Antares::Optimization
{

namespace
{
std::optional<unsigned> BlockTimeIndex(const FillContext& fillContext, unsigned timeIndex)
{
    if (timeIndex >= fillContext.getGlobalFirstTimeStep()
        && timeIndex <= fillContext.getGlobalLastTimeStep())
    {
        return timeIndex - fillContext.getGlobalFirstTimeStep() + 1;
    }
    return std::nullopt;
}

void AddExtraOutputEntry(SimulationTable& simulationTable,
                         const std::string& output,
                         const LegacyVariableInfo& info,
                         double value,
                         const FillContext& fillContext,
                         unsigned currentBlock)
{
    simulationTable.addEntry({.block = currentBlock + 1,
                              .component = info.component,
                              .output = output,
                              .absolute_time_index = info.timeIndex + 1,
                              .block_time_index = BlockTimeIndex(fillContext, info.timeIndex),
                              .scenario_index = fillContext.getYear(),
                              .value = value,
                              .status = std::nullopt});
}

// prop_cost = generation_cost * generation_power: both factors belong to the
// generation variable itself, so they are read by index, which also keeps
// identically-named clusters of different areas apart.
void AddThermalPropCost(SimulationTable& simulationTable,
                        const LegacyVariableInfo& info,
                        std::size_t index,
                        const std::vector<double>& solutionValues,
                        const std::vector<double>& linearCosts,
                        const FillContext& fillContext,
                        unsigned currentBlock)
{
    AddExtraOutputEntry(simulationTable,
                        "prop_cost",
                        info,
                        linearCosts[index] * solutionValues[index],
                        fillContext,
                        currentBlock);
}

// imbalance_cost = spillage_cost * spilled_energy
//                  + unsupplied_energy_cost * unsupplied_energy
// Driven by the area's UnsuppliedEnergy variable; the Spillage variable of the
// same area and timestep is found through the solution view.
void AddAreaImbalanceCost(SimulationTable& simulationTable,
                          const LegacyVariableInfo& info,
                          std::size_t index,
                          const std::vector<double>& solutionValues,
                          const std::vector<double>& linearCosts,
                          const LegacySolutionView& solution,
                          const FillContext& fillContext,
                          unsigned currentBlock)
{
    const auto spilled = solution.value("Spillage", info.component, info.timeIndex);
    const auto spillageCost = solution.linearCost("Spillage", info.component, info.timeIndex);
    if (!spilled || !spillageCost)
    {
        return;
    }

    const double value = spillageCost.value() * spilled.value()
                         + linearCosts[index] * solutionValues[index];
    AddExtraOutputEntry(simulationTable, "imbalance_cost", info, value, fillContext, currentBlock);
}

// is_loss_of_load = 1 when the area has unsupplied energy (above the 0.5 MW
// solver-noise threshold), 0 otherwise.
void AddAreaIsLossOfLoad(SimulationTable& simulationTable,
                         const LegacyVariableInfo& info,
                         std::size_t index,
                         const std::vector<double>& solutionValues,
                         const FillContext& fillContext,
                         unsigned currentBlock)
{
    constexpr double lossOfLoadthreshold = 0.5;
    AddExtraOutputEntry(simulationTable,
                        "is_loss_of_load",
                        info,
                        solutionValues[index] > lossOfLoadthreshold ? 1. : 0.,
                        fillContext,
                        currentBlock);
}

// actual_num_units_on = ceil(num_units_on): the NODU variable may be
// fractional when the unit-commitment problem is relaxed.
void AddThermalActualNumUnitsOn(SimulationTable& simulationTable,
                                const LegacyVariableInfo& info,
                                std::size_t index,
                                const std::vector<double>& solutionValues,
                                const FillContext& fillContext,
                                unsigned currentBlock)
{
    AddExtraOutputEntry(simulationTable,
                        "actual_num_units_on",
                        info,
                        std::ceil(solutionValues[index]),
                        fillContext,
                        currentBlock);
}

// abs_flow = |flow|: the DirectFlow variable is signed (negative when the
// link is used from destination to origin).
void AddLinkAbsFlow(SimulationTable& simulationTable,
                    const LegacyVariableInfo& info,
                    std::size_t index,
                    const std::vector<double>& solutionValues,
                    const FillContext& fillContext,
                    unsigned currentBlock)
{
    AddExtraOutputEntry(simulationTable,
                        "abs_flow",
                        info,
                        std::abs(solutionValues[index]),
                        fillContext,
                        currentBlock);
}

// prop_cost (link) = direct_hurdle_cost * positive_direct_flow
//                    + indirect_hurdle_cost * positive_indirect_flow
// The hurdle costs are the linear objective coefficients on the flow
// decomposition variables (opt_gestion_des_couts_cas_lineaire.cpp), which
// only exist for links managed with hurdle costs. Driven by the link's
// PositiveDirectFlow variable; the PositiveIndirectFlow variable of the same
// link and timestep is found through the solution view.
void AddLinkPropCost(SimulationTable& simulationTable,
                     const LegacyVariableInfo& info,
                     std::size_t index,
                     const std::vector<double>& solutionValues,
                     const std::vector<double>& linearCosts,
                     const LegacySolutionView& solution,
                     const FillContext& fillContext,
                     unsigned currentBlock)
{
    const auto indirect = solution.value("PositiveIndirectFlow", info.component, info.timeIndex);
    const auto indirectCost = solution.linearCost("PositiveIndirectFlow",
                                                  info.component,
                                                  info.timeIndex);
    if (!indirect || !indirectCost)
    {
        return;
    }

    const double value = linearCosts[index] * solutionValues[index]
                         + indirectCost.value() * indirect.value();
    AddExtraOutputEntry(simulationTable, "prop_cost", info, value, fillContext, currentBlock);
}
} // namespace

void AddLegacyExtraOutputs(SimulationTable& simulationTable,
                           const std::vector<std::optional<LegacyVariableInfo>>& variablesInfo,
                           const std::vector<double>& solutionValues,
                           const std::vector<double>& linearCosts,
                           const FillContext& fillContext,
                           unsigned currentBlock)
{
    const LegacySolutionView solution(variablesInfo, solutionValues, linearCosts);

    for (std::size_t index = 0; index < variablesInfo.size(); ++index)
    {
        const auto& info = variablesInfo[index];
        if (!info)
        {
            continue;
        }

        if (info->name == "DispatchableProduction")
        {
            AddThermalPropCost(simulationTable,
                               *info,
                               index,
                               solutionValues,
                               linearCosts,
                               fillContext,
                               currentBlock);
        }
        else if (info->name == "UnsuppliedEnergy")
        {
            AddAreaImbalanceCost(simulationTable,
                                 *info,
                                 index,
                                 solutionValues,
                                 linearCosts,
                                 solution,
                                 fillContext,
                                 currentBlock);
            AddAreaIsLossOfLoad(simulationTable,
                                *info,
                                index,
                                solutionValues,
                                fillContext,
                                currentBlock);
        }
        else if (info->name == "NODU")
        {
            AddThermalActualNumUnitsOn(simulationTable,
                                       *info,
                                       index,
                                       solutionValues,
                                       fillContext,
                                       currentBlock);
        }
        else if (info->name == "DirectFlow")
        {
            AddLinkAbsFlow(simulationTable,
                           *info,
                           index,
                           solutionValues,
                           fillContext,
                           currentBlock);
        }
        else if (info->name == "PositiveDirectFlow")
        {
            AddLinkPropCost(simulationTable,
                            *info,
                            index,
                            solutionValues,
                            linearCosts,
                            solution,
                            fillContext,
                            currentBlock);
        }
    }
}

} // namespace Antares::Optimization
