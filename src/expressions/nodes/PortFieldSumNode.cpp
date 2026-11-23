// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/expressions/nodes/PortFieldSumNode.h>

namespace Antares::Expressions::Nodes
{
namespace
{
std::vector<Node*> operandsFrom(Node* no_connection_expr)
{
    if (no_connection_expr)
    {
        return {no_connection_expr};
    }
    return {};
}
} // namespace

PortFieldSumNode::PortFieldSumNode(const std::string& port_name,
                                   const std::string& field_name,
                                   Node* no_connection_expr):
    ParentNode(operandsFrom(no_connection_expr)),
    port_name_(port_name),
    field_name_(field_name)
{
}

const std::string& PortFieldSumNode::getPortName() const
{
    return port_name_;
}

const std::string& PortFieldSumNode::getFieldName() const
{
    return field_name_;
}

Node* PortFieldSumNode::noConnectionExpr() const
{
    return size() ? (*this)[0] : nullptr;
}
} // namespace Antares::Expressions::Nodes
