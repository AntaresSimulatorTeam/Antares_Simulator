// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <numeric>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/VariabilityVisitor.h>
#include "antares/expressions/visitors/PrintVisitor.h"

using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Optimisation;

namespace Antares::Expressions::Visitors
{

VariabilityType VariabilityVisitor::visitChildrenNodes(const Nodes::ParentNode* node)
{
    const auto visitedChildren = NodeVisitor<VariabilityType>::visitChildrenNodes(node);
    return std::accumulate(visitedChildren.begin(),
                           visitedChildren.end(),
                           VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO,
                           [](auto a, auto b) { return a | b; });
}

VariabilityType VariabilityVisitor::visit(const Nodes::SumNode* node)
{
    return visitChildrenNodes(node);
}

VariabilityType VariabilityVisitor::visit(const Nodes::SubtractionNode* sub)
{
    return dispatch(sub->left()) | dispatch(sub->right());
}

VariabilityType VariabilityVisitor::visit(const Nodes::MultiplicationNode* mult)
{
    return dispatch(mult->left()) | dispatch(mult->right());
}

VariabilityType VariabilityVisitor::visit(const Nodes::DivisionNode* div)
{
    return dispatch(div->left()) | dispatch(div->right());
}

VariabilityType VariabilityVisitor::visit(const Nodes::EqualNode* equ)
{
    return dispatch(equ->left()) | dispatch(equ->right());
}

VariabilityType VariabilityVisitor::visit(const Nodes::LessThanOrEqualNode* lt)
{
    return dispatch(lt->left()) | dispatch(lt->right());
}

VariabilityType VariabilityVisitor::visit(const Nodes::GreaterThanOrEqualNode* gt)
{
    return dispatch(gt->left()) | dispatch(gt->right());
}

VariabilityType VariabilityVisitor::visit(const Nodes::VariableNode* var)
{
    return var->variability();
}

VariabilityType VariabilityVisitor::visit(const Nodes::ParameterNode* param)
{
    const auto systemParameter = evalContext_.getParameter(param->value());
    return systemParameter.type;
}

VariabilityType VariabilityVisitor::visit([[maybe_unused]] const Nodes::LiteralNode* lit)
{
    return VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO;
}

VariabilityType VariabilityVisitor::visit(const Nodes::NegationNode* neg)
{
    return dispatch(neg->child());
}

VariabilityType VariabilityVisitor::visit(const Nodes::PortFieldNode* node)
{
    std::string portId = node->getPortName();
    std::string fieldId = node->getFieldName();

    const auto* nodeToVisit = component_.nodeAtPortField(portId, fieldId);
    return dispatch(nodeToVisit);
}

VariabilityType VariabilityVisitor::visit(const Nodes::PortFieldSumNode* node)
{
    std::string portId = node->getPortName();
    std::string fieldId = node->getFieldName();

    auto to_return = VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO;
    for (const auto connexion_end: component_.componentConnectionsViaPort(portId))
    {
        auto* component = connexion_end.component();
        auto* port = connexion_end.port();

        VariabilityVisitor visitor(optimEntityContainer_, *component, data_, scenario_);
        const Nodes::Node* node = component->nodeAtPortField(port->Id(), fieldId);
        to_return = to_return | visitor.dispatch(node);
    }
    return to_return;
}

VariabilityType VariabilityVisitor::visit(const Nodes::TimeShiftNode* timeShiftNode)
{
    return dispatch(timeShiftNode->left());
}

VariabilityType VariabilityVisitor::visit(
  [[maybe_unused]] const Nodes::TimeIndexNode* timeIndexNode)
{
    return VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO;
}

VariabilityType VariabilityVisitor::visit(const Nodes::TimeSumNode* timeSumNode)
{
    // TODO  case from = to
    return dispatch(timeSumNode->expression());
}

VariabilityType VariabilityVisitor::visit([[maybe_unused]] const Nodes::AllTimeSumNode* timeSumNode)
{
    return VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO;
}

VariabilityType VariabilityVisitor::visitReducedCost(const Nodes::FunctionNode* node)
{
    return dispatch(node->getOperands()[0]);
}

VariabilityType VariabilityVisitor::visitDual(const Nodes::FunctionNode* node)
{
    const auto constraintIndexNode = dynamic_cast<Nodes::LiteralNode*>(node->getOperands().at(1));
    unsigned int constraintIndex = static_cast<unsigned int>(constraintIndexNode->value());
    return optimEntityContainer_.getConstraintVariability(component_, constraintIndex);
}

VariabilityType VariabilityVisitor::visitPow(const Nodes::FunctionNode* node)
{
    const auto* exponent = node->getOperands().at(1);
    if (!isConstant(dispatch(exponent)))
    {
        PrintVisitor visitor;
        throw std::runtime_error("This exponent must be constant in :" + visitor.dispatch(node));
    }
    return dispatch(node->getOperands().at(0));
}

VariabilityType VariabilityVisitor::visit(const Nodes::FunctionNode* node)
{
    switch (node->type())
    {
    case Nodes::FunctionNodeType::reduced_cost:
        return visitReducedCost(node);
    case Nodes::FunctionNodeType::dual:
        return visitDual(node);
    case Nodes::FunctionNodeType::max:
    case Nodes::FunctionNodeType::min:
    case Nodes::FunctionNodeType::floor:
    case Nodes::FunctionNodeType::ceil:
        return visitChildrenNodes(node);
    case Nodes::FunctionNodeType::pow:
        return visitPow(node);
    default:
        return VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO;
    }
}

VariabilityVisitor::VariabilityVisitor(const OptimEntityContainer& optimEntityContainer,
                                       const ModelerStudy::SystemModel::Component& component,
                                       const LinearProblemApi::ILinearProblemData* data,
                                       const LinearProblemApi::IScenario* scenario):
    optimEntityContainer_(optimEntityContainer),
    component_(component),
    data_(data),
    scenario_(scenario),
    evalContext_(&component, data, scenario)
{
}

std::string VariabilityVisitor::name() const
{
    return "TimeIndexVisitor";
}

} // namespace Antares::Expressions::Visitors
