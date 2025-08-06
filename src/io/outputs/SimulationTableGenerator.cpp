/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include "antares/io/outputs/SimulationTableGenerator.h"

#include <antares/solver/optim-model-filler/VariableDictionary.h>
#include "antares/optimisation/linear-problem-api/linearProblem.h"
#include "antares/optimisation/linear-problem-api/mipConstraint.h"
#include "antares/optimisation/linear-problem-api/mipSolution.h"

using namespace Antares::Optimisation::LinearProblemApi;

TimeBlock convertTimeStepToBlockTimeIndex(unsigned int timeStep, const TimeConversionMode& mode)
{
    switch (mode)
    {
    case TimeConversionMode::WeeklyBlocks:
        return {.block = timeStep / 168 + 1,
                .blockTimeIndex = timeStep % 168 + 1,
                .absoluteTimeIndex = timeStep + 1};
    case TimeConversionMode::DailyBlocks:
        return {.block = timeStep / 24 + 1,
                .blockTimeIndex = timeStep % 24 + 1,
                .absoluteTimeIndex = timeStep + 1};
    case TimeConversionMode::SingleBlock:
    default:
        return {.block = 1, .blockTimeIndex = timeStep + 1, .absoluteTimeIndex = timeStep + 1};
    }
}

struct ModelerSolverTraits
{
    static double getValue(const Antares::Optimisation::LinearProblemApi::IMipVariable* var)
    {
        return var->solutionValue();
    }

    static MipBasisStatus getStatus(
      const Antares::Optimisation::LinearProblemApi::IMipVariable* var)
    {
        return var->getMipBasisStatus();
    }

    static MipBasisStatus getStatus(
      const Antares::Optimisation::LinearProblemApi::IMipConstraint* cst)
    {
        return cst->getMipBasisStatus();
    }

    static std::optional<double> getValue(
      const Antares::Optimisation::LinearProblemApi::IMipConstraint*)
    {
        return std::nullopt;
    }
};

std::string BuildModelerConstraintName(const std::string& cid,
                                       const std::string& cname,
                                       const std::optional<unsigned>& scen,
                                       const std::optional<unsigned>& ts)
{
    // TODO
    //  if (scen)
    //      key += "_s" + std::to_string(*scen);
    //  if (ts)
    //      key += "_t" + std::to_string(*ts);

    // TODO
    std::string key = cid + "." + cname;
    if (ts)
    {
        key += "_" + std::to_string(*ts); // TODO
    }
    else if (scen)
    {
        key += "_" + std::to_string(*scen); // TODO
    }
    return key;
}

void FillSimulationTable(
  ISimulationTable& simulationTable,
  const Antares::Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
  const Antares::Optimisation::LinearProblemApi::IMipSolution& solution,
  const std::unordered_map<std::string, Antares::ModelerStudy::SystemModel::Component>& components,
  const Antares::Optimization::VariableDictionary& variableDictionary,
  const Antares::Optimisation::LinearProblemApi::FillContext& fillContext)
{
    auto variableLookupModeler = [&](const std::string& cid,
                                     const std::string& vname,
                                     std::optional<unsigned> scen,
                                     std::optional<unsigned> ts)
      -> const Antares::Optimisation::LinearProblemApi::IMipVariable*
    { return variableDictionary(cid, vname, scen.value_or(0), ts.value_or(0)); };

    auto constraintLookupModeler = [&](const std::string& cid,
                                       const std::string& cname,
                                       std::optional<unsigned> scen,
                                       std::optional<unsigned> ts)
      -> const Antares::Optimisation::LinearProblemApi::IMipConstraint*
    { return linearProblem.lookupConstraint(BuildModelerConstraintName(cid, cname, scen, ts)); };

    for (const auto& component: components | std::views::values)
    {
        // TODO
        unsigned scenario = 0;
        addVariableEntries<ModelerSolverTraits>(simulationTable,
                                                fillContext,
                                                component,
                                                variableLookupModeler,
                                                1,
                                                TimeConversionMode::SingleBlock,
                                                scenario);

        addConstraintEntries<ModelerSolverTraits>(simulationTable,
                                                  fillContext,
                                                  component,
                                                  constraintLookupModeler,
                                                  1,
                                                  TimeConversionMode::SingleBlock,
                                                  scenario);
    }
}
