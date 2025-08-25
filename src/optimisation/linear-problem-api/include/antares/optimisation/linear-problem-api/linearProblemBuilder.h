/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include <vector>
#include <algorithm>

#include "linearProblemFiller.h"

namespace Antares::Optimisation::LinearProblemApi
{

template<SolverTag SolverTagType>
class LinearProblemBuilder
{
public:
    explicit LinearProblemBuilder(const std::vector<LinearProblemFiller<SolverTagType>*>& fillers);
    void build(ILinearProblem<SolverTagType>& pb, ILinearProblemData& data, FillContext& ctx);

private:
    const std::vector<LinearProblemFiller<SolverTagType>*>& fillers_;
};

template<SolverTag SolverTagType>
LinearProblemBuilder<SolverTagType>::LinearProblemBuilder(
  const std::vector<LinearProblemFiller<SolverTagType>*>& fillers):
    fillers_(fillers)
{
}

template<SolverTag SolverTagType>
void LinearProblemBuilder<SolverTagType>::build(ILinearProblem<SolverTagType>& pb,
                                                ILinearProblemData& data,
                                                FillContext& ctx)
{
    std::ranges::for_each(fillers_,
                          [&](const auto& filler) { filler->addVariables(pb, data, ctx); });
    std::ranges::for_each(fillers_,
                          [&](const auto& filler) { filler->addConstraints(pb, data, ctx); });
    std::ranges::for_each(fillers_,
                          [&](const auto& filler) { filler->addObjective(pb, data, ctx); });
}

} // namespace Antares::Optimisation::LinearProblemApi
