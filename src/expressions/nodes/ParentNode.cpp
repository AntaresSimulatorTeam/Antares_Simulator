// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/expressions/nodes/ParentNode.h>

namespace Antares::Expressions::Nodes
{

ParentNode::ParentNode(const std::vector<Node*>& operands):
    operands_(operands)
{
}

const std::vector<Node*>& ParentNode::getOperands() const
{
    return operands_;
}

const std::vector<const Node*> ParentNode::getConstOperands() const
{
    std::vector<const Node*> constOperands(operands_.begin(), operands_.end());
    return constOperands;
}

size_t ParentNode::size() const
{
    return operands_.size();
}

Node* ParentNode::operator[](std::size_t idx) const
{
    return operands_[idx];
}

} // namespace Antares::Expressions::Nodes
