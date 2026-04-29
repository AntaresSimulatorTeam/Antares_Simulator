// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/expressions/nodes/FunctionNode.h"

namespace Antares::Expressions::Nodes
{

/**
 * @brief Convert a FunctionNodeType enum value into its string representation.
 *
 * @param type The function type to convert.
 * @return String representation (e.g. "reduced_cost", "dual", ...).
 */
std::string FunctionNodeTypeToString(FunctionNodeType type)
{
    switch (type)
    {
    case FunctionNodeType::reduced_cost:
        return "reduced_cost";
    case FunctionNodeType::dual:
        return "dual";
    case FunctionNodeType::max:
        return "max";
    case FunctionNodeType::min:
        return "min";
    case FunctionNodeType::pow:
        return "pow";
    case FunctionNodeType::floor:
        return "floor";
    case FunctionNodeType::ceil:
        return "ceil";
    default:
        return "Unknown function";
    }
}

FunctionNode::FunctionNode(FunctionNodeType type, const std::vector<Node*>& operands):
    ParentNode(operands),
    type_(type)
{
}

FunctionNode::FunctionNode(FunctionNodeType type, std::vector<Node*>&& operands):
    ParentNode(std::move(operands)),
    type_(type)
{
}

std::string FunctionNode::name() const
{
    return "FunctionNode::" + FunctionNodeTypeToString(type_);
}

std::string FunctionNode::typeToString() const
{
    return FunctionNodeTypeToString(type_);
}

} // namespace Antares::Expressions::Nodes
