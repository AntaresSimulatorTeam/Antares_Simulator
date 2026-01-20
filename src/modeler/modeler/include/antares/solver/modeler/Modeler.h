// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <antares/optimisation/linear-problem-api/linearProblem.h>
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/solver/modeler/parameters/modelerParameters.h"

#include "ModelerData.h"

namespace Antares::Solver
{
class ILoader;
class IWriter;

class Modeler final
{
public:
    Modeler(ILoader& loader, IWriter& writer);
    void run();

    class ModelerError: public std::runtime_error
    {
    public:
        explicit ModelerError(const std::string& s):
            runtime_error(s)
        {
        }
    };

    ILoader& loader_;
    IWriter& writer_;

    [[nodiscard]] const std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem>&
    masterProblem() const
    {
        return masterProblem_;
    }

    [[nodiscard]] const std::vector<
      std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem>>&
    subproblems() const
    {
        return subproblems_;
    }

private:
    std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem> masterProblem_ = nullptr;
    std::vector<std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem>> subproblems_;
    ModelerParameters parameters_;
    ModelerData data_;
};
} // namespace Antares::Solver
