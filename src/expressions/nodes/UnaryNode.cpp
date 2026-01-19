// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/expressions/nodes/UnaryNode.h>

namespace Antares::Expressions::Nodes
{
UnaryNode::UnaryNode(Node* child):
    ParentNode(child)
{
}

Node* UnaryNode::child() const
{
    return operator[](0);
}
} // namespace Antares::Expressions::Nodes
