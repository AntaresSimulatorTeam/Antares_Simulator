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

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include <antares/solver/optim-model-filler/FullKey.h>

namespace Antares::Optimization
{
using FullKeyMap = std::unordered_map<FullKey, double, FullKeyHash>;

/**
 * @brief Element-wise sum of two maps, with an optional transformation applied to the values of the
 * right-hand-side map.
 *
 * This function performs an element-wise sum of two maps. For each key present in either map:
 * - If the key exists in both maps, the value in the result map is computed as `left_value +
 * op(right_value)`.
 * - If the key exists only in the right map, the value in the result map is computed as
 * `op(right_value)`.
 * - If the key exists only in the left map, the value in the result map is copied from the left
 * map.
 *
 * The function is generic and works with any map-like container that supports the following
 * operations:
 * - `contains(key)`: Checks if a key exists in the map.
 * - `operator[](key)`: Accesses or inserts a value for a key.
 * - `+=`: Adds the value to an existing value in the map (must be supported by the mapped type).
 *
 * @tparam MapType The type of the map (e.g., `std::map`, `std::unordered_map`).
 * @tparam UnaryOp The type of the transformation function applied to the values of the
 * right-hand-side map. Defaults to `std::identity`.
 *
 * @param left The left-hand-side map.
 * @param right The right-hand-side map.
 * @param op A unary operation to transform the values of the right-hand-side map before adding
 * them. Defaults to the identity function.
 * @return None, the left map is modified in-place to contain the element-wise sum of the two input
 * maps.
 *
 * @example
 * Example 1: Using `std::unordered_map<FullKey, double, FullKeyHash>`
 *
 * ```cpp
 *
 * std::unordered_map<FullKey, double, FullKeyHash> map1 = {
 *     {FullKey("component1", "variable1"), 1.0},
 *     {FullKey("component2", "variable2"), 2.0}
 * };
 *
 * std::unordered_map<FullKey, double, FullKeyHash> map2 = {
 *     {FullKey("component1", "variable1"), 3.0},
 *     {FullKey("component3", "variable3"), 4.0}
 * };
 *
 * add_maps(map1, map2);
 * ```
 *
 * @example
 * Example 2: Using `std::map<unsigned int, LinearExpression>`
 *
 * ```cpp
 *
 * std::map<unsigned int, LinearExpression> map3 = {
 *     {1, linearExpression1},
 *     {2, linearExpression2}
 * };
 *
 * std::map<unsigned int, LinearExpression> map4 = {
 *     {1, linearExpression3},
 *     {3, linearExpression4}
 * };
 *
 * add_maps(map3, map4);
 * ```
 */
template<typename MapType, typename UnaryOp = std::identity>
void add_maps(MapType& left, const MapType& right, UnaryOp op = std::identity{})
{
    for (const auto& [key, value]: right)
    {
        auto it = left.find(key);
        if (it != left.end())
        {
            // Key exists in left, add the values
            it->second += op(value);
        }
        else
        {
            // Key does not exist in left, insert the pair
            left.emplace(key, op(value));
        }
    }
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
    /// Build a linear expression with a given offset and a given map of non-zero coefficients
    /// per variable ID
    LinearExpression(double offset, FullKeyMap coef_per_var);
    /// Sum two linear expressions
    LinearExpression operator+(const LinearExpression& other) const;
    /// Subtract two linear expressions
    LinearExpression operator-(const LinearExpression& other) const;
    /// Multiply two linear expressions
    /// Only one can have non-zero coefficients, otherwise the result cannot be linear
    LinearExpression operator*(const LinearExpression& other) const;
    /// Divide two linear expressions
    /// Only first expression can have non-zero coefficients, otherwise the result cannot be
    /// linear
    LinearExpression operator/(const LinearExpression& other) const;
    /// Multiply linear expression by -1
    LinearExpression operator-() const;
    /// Get the offset
    double offset() const;

    /// Get the non-zero coefficients per variable ID
    const FullKeyMap& coefPerVar() const;

    LinearExpression& operator+=(const LinearExpression& value);

    using RawTerm = std::pair<FullKey, double>;
    double offset_ = 0;
    std::vector<RawTerm> terms_; // may contain duplicates
    mutable FullKeyMap cache_;   // aggregated unique sums
    mutable bool cacheValid_ = false;
    /// Mise à l'échelle d'un ensemble de termes (utilitaire)
    static std::vector<RawTerm> scaleTerms(const std::vector<RawTerm>& src, double factor);

    void invalidate()
    {
        cacheValid_ = false;
    }

    /// Construction paresseuse de cache_ si nécessaire
    void materialize() const;
};
} // namespace Antares::Optimization
