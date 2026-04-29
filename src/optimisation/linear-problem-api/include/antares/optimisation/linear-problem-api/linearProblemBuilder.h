// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <vector>

#include "linearProblemFiller.h"

namespace Antares::Optimisation::LinearProblemApi
{

class LinearProblemBuilder final
{
public:
    explicit LinearProblemBuilder(std::vector<std::unique_ptr<LinearProblemFiller>>& fillers);
    void build(const FillContext& ctx);

private:
    const std::vector<std::unique_ptr<LinearProblemFiller>> fillers_;
};

} // namespace Antares::Optimisation::LinearProblemApi
