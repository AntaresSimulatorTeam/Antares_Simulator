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

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

namespace Antares::Optimization
{
// Index compact vers le vecteur global de variables
using VarIndex = std::uint32_t;
using VarIndexMap = std::unordered_map<VarIndex, double>;

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
 * Example 1: Using `std::unordered_map<VarIndex, double>`
 *
 * ```cpp
 *
 * std::unordered_map<VarIndex, double> map1 = {
 *     {1, 1.0},
 *     {2, 2.0}
 * };
 *
 * std::unordered_map<VarIndex, double> map2 = {
 *     {1, 3.0},
 *     {3, 4.0}
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
 * Mise à l'échelle élément-par-élément d'une map d'index.
 * For every key: final_value = scale * initial_value
 * @param map The [VarIndex, double] map to scale
 * @param scale The scale
 * @return The scaled map
 */
VarIndexMap scale_map(const VarIndexMap& map, double scale);

/**
 * LinearExpression
 * Représente une expression linéaire: coefficients non nuls par index de variable + offset.
 * It can be fully defined by:
 * - the non-zero coefficients of the variables (by VarIndex)
 * - a scalar offset
 */
class LinearExpression
{
public:
    /// Build a linear expression with zero offset and zero coefficients
    LinearExpression() = default;
    /// Build a linear expression with a given offset and a given map of non-zero coefficients
    /// per variable index
    LinearExpression(double offset, VarIndexMap coef_per_index);

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
    double offset() const
    {
        return offset_;
    }

    /// Get the non-zero coefficients per variable index (agrégation paresseuse)
    const VarIndexMap& coefPerIndex() const;

    LinearExpression& operator+=(const LinearExpression& value);

    // Termes bruts [index, coefficient], peuvent contenir des doublons
    using RawTerm = std::pair<VarIndex, double>;

    // Mise à l'échelle des termes bruts
    static std::vector<RawTerm> scaleTerms(const std::vector<RawTerm>& src, double factor);

    // Invalidation explicite du cache
    void invalidate()
    {
        cacheValid_ = false;
    }

    // Matérialisation du cache [agrégation des doublons]
    void materialize() const;

private:
    double offset_ = 0.0;
    std::vector<RawTerm> terms_; // chemin d'écriture rapide, doublons possibles
    mutable VarIndexMap cache_;  // somme agrégée par index
    mutable bool cacheValid_ = false;
};
} // namespace Antares::Optimization
