// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/expressions/nodes/TimeSumNode.h>

namespace Antares::Expressions::Nodes
{
TimeSumNode::TimeSumNode(Node* from, Node* to, Node* expression):
    ParentNode(from, to, expression)
{
}

std::string TimeSumNode::name() const
{
    return "TimeSumNode";
}

Node* TimeSumNode::from() const
{
    return operator[](0);
}

Node* TimeSumNode::to() const
{
    return operator[](1);
}

Node* TimeSumNode::expression() const
{
    return operator[](2);
}
} // namespace Antares::Expressions::Nodes
