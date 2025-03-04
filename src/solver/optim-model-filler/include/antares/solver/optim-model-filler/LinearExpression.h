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

#include <antares/solver/optim-model-filler/FullKey.h>

namespace Antares::Optimization
{

template<typename T>
struct IdentityFunction
{
    const T& operator()(const T& t)
    {
        return t;
    }
};

using FullKeyMap = std::unordered_map<FullKey, double, FullKeyHash>;

/**
 * Element-wise sum of two [string, double] maps, preceded an element-wise multiplication of the
 * right-hand-side map. Keys that do not exist in one of the two maps are considered to have a zero
 * value. For every key: value = left_value + rhs_multiplier * right_value
 * @param left The left hand side map
 * @param right The right hand side map
 * @return The map resulting from the operation
 */
template<typename Key,
         typename Value,
         typename UnaryOp = IdentityFunction<Value>,
         typename HashType = std::hash<Key>>
std::unordered_map<Key, Value, HashType> add_maps(
  const std::unordered_map<Key, Value, HashType>& left,
  const std::unordered_map<Key, Value, HashType>& right,
  UnaryOp op = IdentityFunction<Value>{})
{
    auto result(left);
    for (auto [key, value]: right)
    {
        if (result.contains(key))
        {
            result[key] += op(value);
        }
        else
        {
            result[key] = op(value);
        }
    }
    return result;
}

/**
 * Element-wise multiplication of a map by a scale.
 * For every key: final_value = scale * initial_value
 * @param map The [string, double] map to scale
 * @param scale The scale
 * @return The scaled map
 */
FullKeyMap scale_map(const FullKeyMap& map, double scale);

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
    LinearExpression(double offset, FullKeyMap coef_per_var);
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
    LinearExpression operator-() const;

    /// Get the offset
    double offset() const;

    /// Get the non-zero coefficients per variable ID
    const FullKeyMap& coefPerVar() const;

    LinearExpression& operator+=(const LinearExpression& value);

private:
    double offset_ = 0;
    FullKeyMap coef_per_var_;
};

} // namespace Antares::Optimization
