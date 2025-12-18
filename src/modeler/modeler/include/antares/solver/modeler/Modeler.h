// Copyright 2007-2025, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// SPDX-License-Identifier: MPL-2.0
// This file is part of Antares-Simulator,
// Adequacy and Performance assessment for interconnected energy networks.
//
// Antares_Simulator is free software: you can redistribute it and/or modify
// it under the terms of the Mozilla Public Licence 2.0 as published by
// the Mozilla Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Antares_Simulator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// Mozilla Public Licence 2.0 for more details.
//
// You should have received a copy of the Mozilla Public Licence 2.0
// along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.

#pragma once
#include <antares/optimisation/linear-problem-api/linearProblem.h>
#include "antares/solver/modeler/parameters/modelerParameters.h"

#include "data.h"

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
    std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem> masterProblem_;
    std::vector<std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem>> subproblems_;
    ModelerParameters parameters_;
    Antares::Modeler::Data data_;
};
} // namespace Antares::Solver
