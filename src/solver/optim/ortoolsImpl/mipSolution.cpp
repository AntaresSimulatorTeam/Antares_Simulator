/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include <antares/logs/logs.h>
#include <antares/solver/optim/ortoolsImpl/mipSolution.h>

namespace Antares::Solver::Optim::OrtoolsImpl
{

static Api::MipStatus convertStatus(operations_research::MPSolver::ResultStatus& status)
{
    switch (status)
    {
        case operations_research::MPSolver::ResultStatus::OPTIMAL:
            return Api::MipStatus::OPTIMAL;
        case operations_research::MPSolver::ResultStatus::FEASIBLE:
            return Api::MipStatus::FEASIBLE;
        case operations_research::MPSolver::ResultStatus::UNBOUNDED:
            return Api::MipStatus::UNBOUNDED;
        default:
            logs.warning() << "Solve returned an error status";
            break;
    }
    return Api::MipStatus::MIP_ERROR;
}

OrtoolsMipSolution::OrtoolsMipSolution(operations_research::MPSolver::ResultStatus& status,
                                       std::shared_ptr<operations_research::MPSolver> solver):
    responseStatus_(convertStatus(status)),
    mpSolver_(solver)
{
    for (const auto* var: mpSolver_->variables())
    {
        solution_.try_emplace(var->name(), var->solution_value());
    }
}

Api::MipStatus OrtoolsMipSolution::getStatus() const
{
    return responseStatus_;
}

double OrtoolsMipSolution::getObjectiveValue() const
{
    return mpSolver_->MutableObjective()->Value();
}

double OrtoolsMipSolution::getOptimalValue(const Api::IMipVariable* var) const
{
    if (!var)
    {
        return 0;
    }

    try
    {
        return solution_.at(var->getName());
    }
    catch (const std::out_of_range& ex)
    {
        logs.warning() << ex.what();
        logs.warning() << "Solution not found for variable: " << var->getName();
    }
    return 0;
}

std::vector<double> OrtoolsMipSolution::getOptimalValues(
  const std::vector<Api::IMipVariable*>& vars) const
{
    std::vector<double> solution;
    solution.reserve(vars.size());

    for (const auto* var: vars)
    {
        solution.push_back(getOptimalValue(var));
    }

    return solution;
}

} // namespace Antares::Solver::Optim::OrtoolsImpl
