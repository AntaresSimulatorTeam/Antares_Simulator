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

#include <map>
#include <string>
#include <vector>

#include "mipVariable.h"

namespace Antares::Optimisation::LinearProblemApi
{

enum class MipStatus
{
    OPTIMAL,
    FEASIBLE,
    UNBOUNDED,
    INFEASIBLE,
    MIP_ERROR
};

/**
 * MipSolution
 * Used to get the return status of the solve
 * Contains the problem's optimal values for each variable
 */
class IMipSolution
{
public:
    virtual ~IMipSolution() = default;

    virtual MipStatus getStatus() const = 0;
    virtual double getObjectiveValue() const = 0;
    virtual double getOptimalValue(const IMipVariable* var) const = 0;
    virtual std::vector<double> getOptimalValues(const std::vector<IMipVariable*>& vars) const = 0;
    virtual const std::map<std::string, double>& getOptimalValues() const = 0;
};

} // namespace Antares::Optimisation::LinearProblemApi
