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

namespace Antares::Optimization
{
/**
 * Linear Expression
 * Represents an expression that is linear in regard to an optimization problem's variables.
 * It can be fully defined by:
 * - the non-zero coefficients of the variables
 * - a scalar offset
 */
class LinearExpression
{
public:
    /// Build a linear expression with zero offset and zero coefficients
    LinearExpression() = default;
    /// Build a linear expression with a given offset and a given map of non-zero coefficients per
    /// variable ID
    LinearExpression(double offset, std::map<std::string, double> coef_per_var);
    /// Sum two linear expressions
    LinearExpression operator+(const LinearExpression& other) const;
    /// Subtract two linear expressions
    LinearExpression operator-(const LinearExpression& other) const;
    /// Multiply two linear expressions
    /// Only one can have non-zero coefficients, otherwise the result cannot be linear
    LinearExpression operator*(const LinearExpression& other) const;
    /// Divide two linear expressions
    /// Only first expression can have non-zero coefficients, otherwise the result cannot be linear
    LinearExpression operator/(const LinearExpression& other) const;
    /// Multiply linear expression by -1
    LinearExpression negate() const;

    /// Get the offset
    double offset() const
    {
        return offset_;
    }

    /// Get the non-zero coefficients per variable ID
    std::map<std::string, double> coefPerVar() const
    {
        return coef_per_var_;
    }

private:
    double offset_ = 0;
    std::map<std::string, double> coef_per_var_;
};
} // namespace Antares::Optimization
