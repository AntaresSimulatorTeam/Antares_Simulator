// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <algorithm>
#include <memory>

#include <antares/optimisation/linear-problem-api/linearProblemBuilder.h>

namespace Antares::Optimisation::LinearProblemApi
{

LinearProblemBuilder::LinearProblemBuilder(
  std::vector<std::unique_ptr<LinearProblemFiller>>& fillers):
    fillers_(std::move(fillers))
{
}

void LinearProblemBuilder::build(const FillContext& ctx)
{
    std::ranges::for_each(fillers_, [&](const auto& filler) { filler->addVariables(ctx); });
    std::ranges::for_each(fillers_, [&](const auto& filler) { filler->addConstraints(ctx); });
    std::ranges::for_each(fillers_, [&](const auto& filler) { filler->addObjectives(ctx); });
}

} // namespace Antares::Optimisation::LinearProblemApi
