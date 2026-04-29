// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/expressions/visitors/InvalidNode.h>

namespace Antares::Expressions::Visitors
{

InvalidNode::InvalidNode(const std::string& node_name):
    std::invalid_argument("Node visitor encountered an invalid node type: " + node_name)
{
}

NodeTypeShouldBeInExtraOutput::NodeTypeShouldBeInExtraOutput(const std::string& node_name):
    std::invalid_argument("This type of node: '" + node_name
                          + "' should only be used in extra outputs expressions")
{
}
} // namespace Antares::Expressions::Visitors
