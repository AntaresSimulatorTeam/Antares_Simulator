// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/expressions/nodes/BinaryNode.h>

namespace Antares::Expressions::Nodes
{
BinaryNode::BinaryNode(Node* left, Node* right):
    ParentNode(left, right)
{
}

Node* BinaryNode::right() const
{
    return operator[](1);
}

Node* BinaryNode::left() const
{
    return operator[](0);
}

} // namespace Antares::Expressions::Nodes
