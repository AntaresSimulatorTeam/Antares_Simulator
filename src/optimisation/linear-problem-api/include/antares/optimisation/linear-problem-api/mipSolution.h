// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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

    [[nodiscard]] virtual MipStatus getStatus() const = 0;
    [[nodiscard]] virtual double getObjectiveValue() const = 0;
};
} // namespace Antares::Optimisation::LinearProblemApi
