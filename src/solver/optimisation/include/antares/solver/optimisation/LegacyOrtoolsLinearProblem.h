// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h>

namespace Antares::Optimization
{

class LegacyOrtoolsLinearProblem final
    : public Optimisation::LinearProblemMpsolverImpl::OrtoolsLinearProblem
{
public:
    LegacyOrtoolsLinearProblem(bool isMip, const std::string& solverName):
        OrtoolsLinearProblem(isMip, solverName)
    {
        // nothing else to do
    }

    std::shared_ptr<operations_research::MPSolver> getMpSolver()
    {
        return mpSolver_;
    }
};

} // namespace Antares::Optimization
