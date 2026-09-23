// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/logs/logs.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/mipSolution.h>
#include <antares/solver/utils/ortools_utils.h>

namespace Antares::LinearProblem::MpsolverImpl
{

OrtoolsMipSolution::OrtoolsMipSolution(MPSolver::ResultStatus& status, MPSolver* solver):
    status_(status),
    mpSolver_(solver)
{
    for (const auto* var: mpSolver_->variables())
    {
        solution_.try_emplace(var->name(), var->solution_value());
    }
}

Api::MipStatus OrtoolsMipSolution::getStatus() const
{
    switch (status_)
    {
    case MPSolver::ResultStatus::OPTIMAL:
        return Api::MipStatus::OPTIMAL;
    case MPSolver::ResultStatus::FEASIBLE:
        return Api::MipStatus::FEASIBLE;
    case MPSolver::ResultStatus::UNBOUNDED:
        return Api::MipStatus::UNBOUNDED;
    case MPSolver::ResultStatus::INFEASIBLE:
        return Api::MipStatus::INFEASIBLE;
    default:
        logs.warning() << "Solve returned an error status";
        break;
    }
    return Api::MipStatus::MIP_ERROR;
}

double OrtoolsMipSolution::getObjectiveValue() const
{
    return ::getObjectiveValue(mpSolver_);
}
} // namespace Antares::LinearProblem::MpsolverImpl
