// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/CloneVisitor.h>

namespace Antares::Expressions::Visitors
{
CloneVisitor::CloneVisitor(Registry<Nodes::Node>& registry):
    registry_(registry)
{
}

Nodes::Node* CloneVisitor::visit(const Nodes::SumNode* node)
{
    auto clonedChildren = visitChildrenNodes(node);
    // Give ownership of cloned children to caller
    return registry_.create<Nodes::SumNode>(std::move(clonedChildren));
}

Nodes::Node* CloneVisitor::visit(const Nodes::SubtractionNode* node)
{
    return registry_.create<Nodes::SubtractionNode>(dispatch(node->left()),
                                                    dispatch(node->right()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::MultiplicationNode* node)
{
    return registry_.create<Nodes::MultiplicationNode>(dispatch(node->left()),
                                                       dispatch(node->right()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::DivisionNode* node)
{
    return registry_.create<Nodes::DivisionNode>(dispatch(node->left()), dispatch(node->right()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::EqualNode* node)
{
    return registry_.create<Nodes::EqualNode>(dispatch(node->left()), dispatch(node->right()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::LessThanOrEqualNode* node)
{
    return registry_.create<Nodes::LessThanOrEqualNode>(dispatch(node->left()),
                                                        dispatch(node->right()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::GreaterThanOrEqualNode* node)
{
    return registry_.create<Nodes::GreaterThanOrEqualNode>(dispatch(node->left()),
                                                           dispatch(node->right()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::NegationNode* negationNode)
{
    return registry_.create<Nodes::NegationNode>(dispatch(negationNode->child()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::VariableNode* variableNode)
{
    return registry_.create<Nodes::VariableNode>(variableNode->value(),
                                                 variableNode->Index(),
                                                 variableNode->variability());
}

Nodes::Node* CloneVisitor::visit(const Nodes::ParameterNode* parameterNode)
{
    return registry_.create<Nodes::ParameterNode>(parameterNode->value());
}

Nodes::Node* CloneVisitor::visit(const Nodes::LiteralNode* literalNode)
{
    return registry_.create<Nodes::LiteralNode>(literalNode->value());
}

Nodes::Node* CloneVisitor::visit(const Nodes::PortFieldNode* portfieldNode)
{
    return registry_.create<Nodes::PortFieldNode>(portfieldNode->getPortName(),
                                                  portfieldNode->getFieldName());
}

Nodes::Node* CloneVisitor::visit(const Nodes::PortFieldSumNode* portfieldSumNode)
{
    return registry_.create<Nodes::PortFieldSumNode>(portfieldSumNode->getPortName(),
                                                     portfieldSumNode->getFieldName());
}

Nodes::Node* CloneVisitor::visit(const Nodes::TimeShiftNode* node)
{
    return registry_.create<Nodes::TimeShiftNode>(dispatch(node->left()), node->right());
}

Nodes::Node* CloneVisitor::visit(const Nodes::TimeIndexNode* node)
{
    return registry_.create<Nodes::TimeIndexNode>(dispatch(node->left()), dispatch(node->right()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::TimeSumNode* node)
{
    return registry_.create<Nodes::TimeSumNode>(dispatch(node->from()),
                                                dispatch(node->to()),
                                                dispatch(node->expression()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::AllTimeSumNode* node)
{
    return registry_.create<Nodes::AllTimeSumNode>(dispatch(node->child()));
}

Nodes::Node* CloneVisitor::visit(const Nodes::FunctionNode* node)
{
    auto clonedChildren = visitChildrenNodes(node);
    // Give ownership of cloned children to caller
    return registry_.create<Nodes::FunctionNode>(node->type(), std::move(clonedChildren));
}

std::string CloneVisitor::name() const
{
    return "CloneVisitor";
}

} // namespace Antares::Expressions::Visitors
