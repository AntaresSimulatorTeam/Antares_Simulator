// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/CompareVisitor.h>

template<class T, class V>
static bool compareParentNode(V& visitor,
                              const T* node,
                              const Antares::Expressions::Nodes::Node* other)
{
    if (const T* other_node = dynamic_cast<const T*>(other))
    {
        if (node->size() != other_node->size())
        {
            return false;
        }
        for (std::size_t i = 0; i < node->size(); ++i)
        {
            if (!visitor.dispatch(node->getOperands()[i], other_node->getOperands()[i]))
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

template<class T>
static bool compareGetValue(const T* node, const Antares::Expressions::Nodes::Node* other)
{
    if (const T* other_node = dynamic_cast<const T*>(other))
    {
        return node->value() == other_node->value();
    }
    return false;
}

template<class T>
static bool compareEqualOperator(const T* node, const Antares::Expressions::Nodes::Node* other)
{
    if (const T* other_node = dynamic_cast<const T*>(other))
    {
        return *node == *other_node;
    }
    return false;
}

namespace Antares::Expressions::Visitors
{
bool CompareVisitor::visit(const Nodes::SumNode* node, const Nodes::Node* other)
{
    return compareParentNode(*this, node, other);
}

bool CompareVisitor::visit(const Nodes::SubtractionNode* node, const Nodes::Node* other)
{
    return compareParentNode(*this, node, other);
}

bool CompareVisitor::visit(const Nodes::MultiplicationNode* node, const Nodes::Node* other)
{
    return compareParentNode(*this, node, other);
}

bool CompareVisitor::visit(const Nodes::DivisionNode* node, const Nodes::Node* other)
{
    return compareParentNode(*this, node, other);
}

bool CompareVisitor::visit(const Nodes::EqualNode* node, const Nodes::Node* other)
{
    return compareParentNode(*this, node, other);
}

bool CompareVisitor::visit(const Nodes::LessThanOrEqualNode* node, const Nodes::Node* other)
{
    return compareParentNode(*this, node, other);
}

bool CompareVisitor::visit(const Nodes::GreaterThanOrEqualNode* node, const Nodes::Node* other)
{
    return compareParentNode(*this, node, other);
}

bool CompareVisitor::visit(const Nodes::NegationNode* node, const Nodes::Node* other)
{
    if (auto* other_node = dynamic_cast<const Nodes::NegationNode*>(other))
    {
        return dispatch(node->child(), other_node->child());
    }
    return false;
}

bool CompareVisitor::visit(const Nodes::ParameterNode* node, const Nodes::Node* other)
{
    return compareGetValue(node, other);
}

bool CompareVisitor::visit(const Nodes::LiteralNode* node, const Nodes::Node* other)
{
    return compareGetValue(node, other);
}

bool CompareVisitor::visit(const Nodes::VariableNode* node, const Nodes::Node* other)
{
    return compareGetValue(node, other);
}

bool CompareVisitor::visit(const Nodes::PortFieldNode* node, const Nodes::Node* other)
{
    return compareEqualOperator(node, other);
}

bool CompareVisitor::visit(const Nodes::PortFieldSumNode* node, const Nodes::Node* other)
{
    return compareEqualOperator(node, other);
}

bool CompareVisitor::visit(const Nodes::TimeShiftNode* timeShiftNode, const Nodes::Node* other)
{
    return compareParentNode(*this, timeShiftNode, other);
}

bool CompareVisitor::visit(const Nodes::TimeIndexNode* timeIndexNode, const Nodes::Node* other)
{
    return compareParentNode(*this, timeIndexNode, other);
}

bool CompareVisitor::visit(const Nodes::TimeSumNode* timeSumNode, const Nodes::Node* other)
{
    return compareParentNode(*this, timeSumNode, other);
}

bool CompareVisitor::visit(const Nodes::AllTimeSumNode* alltimeSumNode, const Nodes::Node* other)
{
    return compareParentNode(*this, alltimeSumNode, other);
}

bool CompareVisitor::visit(const Nodes::FunctionNode* node, const Nodes::Node* other)
{
    auto typeAndChildrenComparison = compareParentNode(*this, node, other);
    // we also need to compare the type of the function max, min etc..
    const auto* other_node = dynamic_cast<const Nodes::FunctionNode*>(other);
    return typeAndChildrenComparison && node->type() == other_node->type();
}

std::string CompareVisitor::name() const
{
    return "CompareVisitor";
}

} // namespace Antares::Expressions::Visitors
