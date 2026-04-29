// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <map>
#include <ortools/linear_solver/linear_solver.h>
#include <string>
#include <vector>

#include <antares/optimisation/linear-problem-api/mipSolution.h>

namespace Antares::Optimisation::LinearProblemMpsolverImpl
{

class OrtoolsMipSolution final: public LinearProblemApi::IMipSolution
{
public:
    OrtoolsMipSolution(operations_research::MPSolver::ResultStatus& responseStatus,
                       operations_research::MPSolver* solver);

    ~OrtoolsMipSolution() override = default;

    [[nodiscard]] LinearProblemApi::MipStatus getStatus() const override;
    [[nodiscard]] double getObjectiveValue() const override;

private:
    operations_research::MPSolver::ResultStatus status_;
    operations_research::MPSolver* mpSolver_;
    std::map<std::string, double> solution_;
};

} // namespace Antares::Optimisation::LinearProblemMpsolverImpl
