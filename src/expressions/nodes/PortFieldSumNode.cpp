// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/expressions/nodes/PortFieldSumNode.h>

namespace Antares::Expressions::Nodes
{
PortFieldSumNode::PortFieldSumNode(const std::string& port_name, const std::string& field_name):
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
} // namespace Antares::Expressions::Nodes
