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

#include <algorithm>
#include <chrono>
#include <iostream>

#include <antares/optimisation/linear-problem-api/linearProblemBuilder.h>

namespace Antares::Optimisation::LinearProblemApi
{

LinearProblemBuilder::LinearProblemBuilder(const std::vector<LinearProblemFiller*>& fillers):
    fillers_(fillers)
{
}

void LinearProblemBuilder::build(ILinearProblem& pb, ILinearProblemData& data, FillContext& ctx)
{
    auto start = std::chrono::system_clock::now();
    std::ranges::for_each(fillers_,
                          [&](const auto& filler) { filler->addVariables(pb, data, ctx); });
    std::ranges::for_each(fillers_,
                          [&](const auto& filler) { filler->addConstraints(pb, data, ctx); });
    std::ranges::for_each(fillers_,
                          [&](const auto& filler) { filler->addObjective(pb, data, ctx); });
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "LinearProblem.build in " << elapsed_seconds.count() << " seconds." << std::endl;
    std::cout << "Total variables : " << pb.variableCount() << std::endl;
    std::cout << "Total constraints : " << pb.constraintCount() << std::endl;
}

} // namespace Antares::Optimisation::LinearProblemApi
