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

#include <stdexcept>

#include <antares/solver/optim-model-filler/LinearExpression.h>
#include "antares/expressions/nodes/ExpressionsNodes.h"
using namespace Antares::Expressions;

namespace Antares::Optimization
{

/**
 * Element-wise sum of two [string, double] maps, preceded an element-wise multiplication of the
 * right-hand-side map. Keys that do not exist in one of the two maps are considered to have a zero
 * value. For every key: value = left_value + rhs_multiplier * right_value
 * @param registry node registry
 * @param left The left hand side map
 * @param right The right hand side map
 * @param rhs_multiplier The multiplier to apply to the right hand side map
 * @return The map resulting from the operation
 */
static std::map<std::string, Nodes::Node*> add_maps(
  Registry<Nodes::Node>& registry,
  const std::map<std::string, Nodes::Node*>& left,
  const std::map<std::string, Nodes::Node*>& right,
  double rhs_multiplier)
{
    std::map result(left);
    for (auto [key, value]: right)
    {
        auto literal_node = registry.create<Nodes::LiteralNode>(rhs_multiplier);
        auto multiplication_node = registry.create<Nodes::MultiplicationNode>(literal_node, value);
        if (result.contains(key))
        {
            result[key] = registry.create<Nodes::SumNode>(result[key], multiplication_node);
        }
        else
        {
            result[key] = multiplication_node;
        }
    }
    return result;
}

/**
 * Element-wise multiplication of a map by a scale.
 * For every key: final_value = scale * initial_value
 * @param registry node registry
 * @param map The [string, Expressions::Nodes::Node*] map to scale
 * @param scale The scale
 * @return The scaled map
 */
static std::map<std::string, Nodes::Node*> scale_map(Registry<Nodes::Node>& registry,
                                                     const std::map<std::string, Nodes::Node*>& map,
                                                     Nodes::Node* scale)
{
    std::map<std::string, Nodes::Node*> result;
    for (auto [key, value]: map)
    {
        result[key] = registry.create<Nodes::MultiplicationNode>(scale, value);
    }
    return result;
}

LinearExpression::LinearExpression(Registry<Nodes::Node>& registry,
                                   Nodes::Node* offset,
                                   std::map<std::string, Nodes::Node*> coef_per_var):
    registry_(registry),
    offset_(offset),
    coef_per_var_(std::move(coef_per_var))
{
}

LinearExpression::LinearExpression(Registry<Nodes::Node>& registry):
    registry_(registry)
{
}

LinearExpression LinearExpression::operator+(const LinearExpression& other) const
{
    return {registry_,
            registry_.create<Nodes::SumNode>(offset_, other.offset_),
            add_maps(registry_, coef_per_var_, other.coef_per_var_, 1)};
}

LinearExpression& LinearExpression::operator+=(const LinearExpression& other)
{
    offset_ = registry_.create<Nodes::SumNode>(offset_, other.offset_);

    coef_per_var_ = add_maps(registry_, coef_per_var_, other.coef_per_var_, 1);

    return *this;
}

LinearExpression LinearExpression::operator-(const LinearExpression& other) const
{
    return {registry_,
            registry_.create<Nodes::SubtractionNode>(offset_, other.offset_),
            add_maps(registry_, coef_per_var_, other.coef_per_var_, -1)};
}

LinearExpression LinearExpression::operator*(const LinearExpression& other) const
{
    if (coef_per_var_.empty())
    {
        return {registry_,
                registry_.create<Nodes::MultiplicationNode>(offset_, other.offset_),
                scale_map(registry_, other.coef_per_var_, offset_)};
    }
    else if (other.coef_per_var_.empty())
    {
        return {registry_,
                registry_.create<Nodes::MultiplicationNode>(offset_, other.offset_),
                scale_map(registry_, coef_per_var_, other.offset_)};
    }
    else
    {
        throw std::invalid_argument("A linear expression can't have quadratic terms.");
    }
}

LinearExpression LinearExpression::operator/(const LinearExpression& other) const
{
    if (!other.coef_per_var_.empty())
    {
        throw std::invalid_argument("A linear expression can't have a variable as a dividend.");
    }
    return LinearExpression(
      registry_,
      registry_.create<Nodes::DivisionNode>(offset_, other.offset_),
      scale_map(registry_,
                coef_per_var_,
                registry_.create<Nodes::DivisionNode>(registry_.create<Nodes::LiteralNode>(1),
                                                      other.offset_)));
}

LinearExpression LinearExpression::negate() const
{
    return {registry_,
            registry_.create<Nodes::NegationNode>(offset()),
            scale_map(registry_, coef_per_var_, registry_.create<Nodes::LiteralNode>(-1))};
}
} // namespace Antares::Optimization
