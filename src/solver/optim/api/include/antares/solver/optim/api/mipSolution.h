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

#include <vector>

#include "mipObjective.h"
#include "mipVariable.h"

namespace Antares::Solver::Optim::Api
{

enum class MipStatus
{
    OPTIMAL,
    FEASIBLE,
    UNBOUNDED,
    ERROR
};

class MipSolution
{
public:
    virtual MipStatus getStatus() = 0;
    virtual double getObjectiveValue() = 0;
    virtual double getOptimalValue(const MipVariable& var) const = 0;
    virtual std::vector<double> getOptimalValue(const std::vector<MipVariable>& vars) const = 0;
};

} // namespace Antares::Solver::Optim::Api
