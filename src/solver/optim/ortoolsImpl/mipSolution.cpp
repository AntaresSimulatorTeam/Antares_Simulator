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

#include <antares/solver/optim/ortoolsImpl/mipSolution.h>

namespace Antares::Solver::Optim::OrtoolsImpl
{

using OrMPSolver = operations_research::MPSolver;

OrtoolsMipSolution::OrtoolsMipSolution(OrMPSolver::ResultStatus& responseStatus,
                                       const std::map<Api::MipVariable*, double>& solution,
                                       OrtoolsMipObjective* objective):
    objective_(objective)
{
    // Only store non-zero values
    for (const auto& varAndValue: solution)
    {
        if (abs(varAndValue.second) > 1e-6) // TODO: is this tolerance OK?
        {
            solution_.insert(varAndValue);
        }
    }

    switch (responseStatus)
    {
    case OrMPSolver::ResultStatus::OPTIMAL:
        responseStatus_ = Api::MipStatus::OPTIMAL;
        break;
    case OrMPSolver::ResultStatus::FEASIBLE:
        responseStatus_ = Api::MipStatus::FEASIBLE;
        break;
    case OrMPSolver::ResultStatus::UNBOUNDED:
        responseStatus_ = Api::MipStatus::UNBOUNDED;
        break;
    default:
        responseStatus_ = Api::MipStatus::ERROR;
        break;
    }
}

Api::MipStatus OrtoolsMipSolution::getStatus()
{
    return responseStatus_;
}

double OrtoolsMipSolution::getObjectiveValue()
{
    return objective_->getMpObjective()->Value();
}

double OrtoolsMipSolution::getOptimalValue(const Api::MipVariable& var) const
{
    return solution_.contains(&var) ? solution_.at(&var) : 0;
}

std::vector<double> OrtoolsMipSolution::getOptimalValue(
  const std::vector<Api::MipVariable>& vars) const
{
    std::vector<double> solution;
    solution.reserve(vars.size());

    for (const auto& var: vars)
    {
        solution.push_back(getOptimalValue(var));
    }

    return solution;
}

} // namespace Antares::Solver::Optim::OrtoolsImpl