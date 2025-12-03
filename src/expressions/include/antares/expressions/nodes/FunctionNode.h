/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include <string>

#include "antares/expressions/nodes/ParentNode.h"

namespace Antares::Expressions::Nodes
{

/**
 * @brief Types of function nodes supported in Antares Modeler expressions.
 */
enum class FunctionNodeType
{
    reduced_cost, ///< Reduced cost of a variable.
    dual,         ///< Dual value associated with a constraint.
    max,          ///< Maximum of multiple expressions.
    min,          ///< Minimum of multiple expressions.
    pow,          ///< Exponentiation: base^exponent.
};

/**
 * @brief Convert a FunctionNodeType enum value into its string representation.
 *
 * @param type The function type to convert.
 * @return String representation (e.g. "reduced_cost", "dual", ...).
 */
std::string FunctionNodeTypeToString(FunctionNodeType type);

/**
 * @brief AST node representing a function expression (max, min, pow, ...).
 *
 * FunctionNode is a ParentNode whose operands represent the arguments of the
 * function. The type indicates which function is applied.
 *  examples:
 * - base^exponent -> FunctionNode(FunctionNodeType::pow, base, exponent)
 * - max(a, b, c) -> FunctionNode(FunctionNodeType::max, a, b, c)
 * - min(x, y) -> FunctionNode(FunctionNodeType::min, x, y)
 * - reduced_cost(var) -> FunctionNode(FunctionNodeType::reduced_cost, VariableNode("var"))
 * - dual(constraint) -> FunctionNode(FunctionNodeType::dual, ParameterNode("constraint"))
 */
class FunctionNode final: public ParentNode
{
public:
    /**
     * @brief Construct a FunctionNode from a list of operands.
     *
     * @tparam NodePtr Variadic parameter pack of Node* operands.
     * @param type Function type.
     * @param operands Node* operands passed to ParentNode.
     */
    template<typename... NodePtr>
    explicit FunctionNode(FunctionNodeType type, NodePtr... operands):
        ParentNode(operands...),
        type_(type)

    {
    }

    /**
     * @brief Construct a FunctionNode from an existing vector of operands.
     *
     * @param type Function type.
     * @param operands Vector of Node* operands.
     */
    explicit FunctionNode(FunctionNodeType type, const std::vector<Node*>& operands):
        ParentNode(operands),
        type_(type)
    {
    }

    /**
     * @brief Construct a FunctionNode by moving operand vector.
     *
     * @param type Function type.
     * @param operands Rvalue vector of operands.
     */
    explicit FunctionNode(FunctionNodeType type, std::vector<Node*>&& operands):
        ParentNode(std::move(operands)),
        type_(type)
    {
    }

    /**
     * @return The name of this node ("FunctionNode").
     */
    std::string name() const override
    {
        return "FunctionNode";
    }

    /**
     * @return Function type associated with this node.
     */
    FunctionNodeType type() const
    {
        return type_;
    }

    /**
     * @return String representation of the function type.
     */
    std::string typeToString() const
    {
        return FunctionNodeTypeToString(type_);
    }

private:
    FunctionNodeType type_; ///< Function type applied to operands.
};
} // namespace Antares::Expressions::Nodes
