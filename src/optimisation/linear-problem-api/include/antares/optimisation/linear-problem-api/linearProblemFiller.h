// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/optimisation/linear-problem-api/linearProblem.h>

namespace Antares::Optimisation::LinearProblemApi
{

class LinearProblemFiller
{
public:
    // TODO it may be better to move the LinearProblemData out of the addVariables / addConstraints
    // / addObjective, and into the ctor because it is unique across the simulation
    virtual void addVariables(const FillContext& ctx) = 0;
    virtual void addConstraints(const FillContext& ctx) = 0;
    virtual void addObjectives(const FillContext& ctx) = 0;
    virtual ~LinearProblemFiller() = default;
};

} // namespace Antares::Optimisation::LinearProblemApi
