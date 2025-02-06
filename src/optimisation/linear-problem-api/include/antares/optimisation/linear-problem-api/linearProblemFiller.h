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

#pragma once

#include <memory>
#include <unordered_map>

#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/optimisation/linear-problem-api/linearProblem.h>

namespace Antares::Expressions::Visitors
{
enum class TimeIndex : unsigned int;
}

namespace Antares::Expressions::Nodes
{
class Node;
}

namespace Antares::Optimisation::LinearProblemApi
{
struct FillContext
{
    FillContext(unsigned first, unsigned last):
        firstTimeStep(first),
        lastTimeStep(last)
    {
    }

    unsigned getFirstTimeStep() const
    {
        return firstTimeStep;
    }

    unsigned getLastTimeStep() const
    {
        return lastTimeStep;
    }

    std::vector<unsigned> scenariosSelected;

    unsigned int getNumberOfTimestep() const
    {
        return lastTimeStep - firstTimeStep + 1;
    }

private:
    unsigned firstTimeStep = 0;
    unsigned lastTimeStep = 0;
    std::unordered_map<const Expressions::Nodes::Node*, Expressions::Visitors::TimeIndex>
      nodesTimeIndex;
};

class LinearProblemFiller
{
public:
    virtual void addVariables(ILinearProblem& pb, ILinearProblemData& data, FillContext& ctx) = 0;
    virtual void addConstraints(ILinearProblem& pb, ILinearProblemData& data, FillContext& ctx) = 0;
    virtual void addObjective(ILinearProblem& pb, ILinearProblemData& data, FillContext& ctx) = 0;
    virtual ~LinearProblemFiller() = default;
};

} // namespace Antares::Optimisation::LinearProblemApi
