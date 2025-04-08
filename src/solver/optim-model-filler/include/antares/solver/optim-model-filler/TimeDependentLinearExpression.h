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

#include <string>
#include <unordered_map>
#include <vector>

#include <antares/solver/optim-model-filler/LinearExpression.h>
#include "antares/optimisation/linear-problem-api/ILinearProblemData.h"

namespace Antares::Optimization
{
using LinearExpressionMap = std::map<unsigned int, LinearExpression>;

// time dependent parameter
class TimeDependentLinearExpression
{
public:
    explicit TimeDependentLinearExpression(
      const Optimisation::LinearProblemApi::FillContext& fillContext);

    // Construction from one LinearExpression, to be duplicated for all timestep
    explicit TimeDependentLinearExpression(
      const Optimisation::LinearProblemApi::FillContext& fillContext,
      const LinearExpression& linearExpression);
    explicit TimeDependentLinearExpression(const LinearExpressionMap& linearExpressions);

    /// Sum two linear expressions
    TimeDependentLinearExpression operator+(const TimeDependentLinearExpression& other) const;
    /// Subtract two linear expressions
    TimeDependentLinearExpression operator-(const TimeDependentLinearExpression& other) const;
    /// Multiply two linear expressions
    /// Only one can have non-zero coefficients, otherwise the result cannot be linear
    TimeDependentLinearExpression operator*(const TimeDependentLinearExpression& other) const;
    /// Divide two linear expressions
    /// Only first expression can have non-zero coefficients, otherwise the result cannot be linear
    TimeDependentLinearExpression operator/(const TimeDependentLinearExpression& other) const;
    /// Multiply linear expression by -1
    TimeDependentLinearExpression operator-() const;
    TimeDependentLinearExpression shiftLinearExpressions(int shiftValue) const;
    TimeDependentLinearExpression operator[](int timeStep) const;
    TimeDependentLinearExpression timeSumLinearExpressions(int from, int to) const;
    TimeDependentLinearExpression allTimeSumLinearExpressions() const;

    const LinearExpressionMap& GetLinearExpressions() const;
    size_t getSize() const;
    TimeDependentLinearExpression& operator+=(const TimeDependentLinearExpression& other);

private:
    LinearExpressionMap linearExpressions_;
};
} // namespace Antares::Optimization
