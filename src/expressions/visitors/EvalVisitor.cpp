// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/expressions/visitors/EvalVisitor.h"

#include <numeric>
#include <stdexcept>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include "antares/expressions/ShiftVector.h"
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"

using namespace Antares::Optimisation;

namespace Antares::Expressions::Visitors
{

EvalVisitor::EvalVisitor(const OptimEntityContainer& optimContainer,
                         const LinearProblemApi::FillContext& fillContext,
                         const ModelerStudy::SystemModel::Component& component,
                         const LinearProblemApi::ILinearProblemData* data,
                         const LinearProblemApi::IScenario* scenario):
    // TODO put component or its id inside context, it is already component-bound.
    // Plus it is mandatory to visit Variables & PortFieldSums
    // Else, create a PostOptimEvalVisitor that inherits from EvalVisitor & has a different ctor
    component_(component),
    data_(data),
    scenario_(scenario),
    optimContainer_(optimContainer),
    evalContext_(&component, data, scenario),
    fillContext_(fillContext)
{
}

EvaluationResult EvalVisitor::visit(const Nodes::SumNode* node)
{
    const auto& operands = node->getOperands();
    return std::accumulate(std::begin(operands),
                           std::end(operands),
                           EvaluationResult{0.},
                           [this](const EvaluationResult& sum, const Nodes::Node* operand)
                           { return sum + dispatch(operand); });
}

EvaluationResult EvalVisitor::visit(const Nodes::SubtractionNode* node)
{
    return dispatch(node->left()) - dispatch(node->right());
}

EvaluationResult EvalVisitor::visit(const Nodes::MultiplicationNode* node)
{
    return dispatch(node->left()) * dispatch(node->right());
}

EvaluationResult EvalVisitor::visit(const Nodes::DivisionNode* node)
{
    return dispatch(node->left()) / dispatch(node->right());
}

EvaluationResult EvalVisitor::visit(const Nodes::EqualNode* node)
{
    return dispatch(node->left()) == dispatch(node->right());
}

EvaluationResult EvalVisitor::visit(const Nodes::LessThanOrEqualNode* node)
{
    return dispatch(node->left()) <= dispatch(node->right());
}

EvaluationResult EvalVisitor::visit(const Nodes::GreaterThanOrEqualNode* node)
{
    return dispatch(node->left()) >= dispatch(node->right());
}

EvaluationResult EvalVisitor::visit(const Nodes::VariableNode* node)
{
    if (isTimeConstant(node->variability()))
    {
        const std::span componentVariables = optimContainer_.getComponentVariable(
          component_,
          node->Index(),
          1 /* single timestep*/);

        return EvaluationResult(componentVariables[0]->solutionValue());
    }
    // VARYING_IN_TIME_ONLY or VARYING_IN_TIME_AND_SCENARIO)
    const unsigned nbTimeStep = fillContext_.getLocalNumberOfTimeSteps();
    std::vector<double> varValues(nbTimeStep, 0.0);
    const std::span componentVariables = optimContainer_.getComponentVariable(component_,
                                                                              node->Index(),
                                                                              nbTimeStep);
    for (unsigned varInd = 0; varInd < nbTimeStep; ++varInd)
    {
        varValues[varInd] = componentVariables[varInd]->solutionValue();
    }

    return EvaluationResult{varValues};
}

EvaluationResult EvalVisitor::visit(const Nodes::ParameterNode* node)
{
    const auto systemParameter = evalContext_.getParameter(node->value());
    if (systemParameter.type == VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO)
    {
        return EvaluationResult{evalContext_.getSystemParameterValueAsDouble(node->value())};
    }
    if (systemParameter.type == VariabilityType::VARYING_IN_SCENARIO_ONLY)
    {
        return EvaluationResult(
          evalContext_.getParameterValue(node->value(), fillContext_.getYear(), 0));
    }

    unsigned year = fillContext_.getYear();
    std::vector<double> params;
    params.reserve(fillContext_.getLocalNumberOfTimeSteps());
    for (auto t = fillContext_.getGlobalFirstTimeStep(); t <= fillContext_.getGlobalLastTimeStep();
         ++t)
    {
        params.emplace_back(evalContext_.getParameterValue(node->value(), year, t));
    }
    return EvaluationResult{params};
}

EvaluationResult EvalVisitor::visit(const Nodes::LiteralNode* node)
{
    return EvaluationResult{node->value()};
}

EvaluationResult EvalVisitor::visit(const Nodes::NegationNode* node)
{
    return -dispatch(node->child());
}

EvaluationResult EvalVisitor::visit(const Nodes::PortFieldNode* node)
{
    std::string portId = node->getPortName();
    std::string fieldId = node->getFieldName();

    const auto* nodeToVisit = component_.nodeAtPortField(portId, fieldId);
    return dispatch(nodeToVisit);
}

EvaluationResult EvalVisitor::visit(const Nodes::PortFieldSumNode* node)
{
    std::string portId = node->getPortName();
    std::string fieldId = node->getFieldName();

    EvaluationResult result(0.);
    for (const auto connectionEnd: component_.componentConnectionsViaPort(portId))
    {
        auto* component = connectionEnd.component();
        auto* port = connectionEnd.port();
        EvalVisitor visitor(optimContainer_, fillContext_, *component, data_, scenario_);
        const auto* nodeToVisit = component->nodeAtPortField(port->Id(), fieldId);
        auto dispatchResult = visitor.dispatch(nodeToVisit);
        result += dispatchResult;
    }
    return result;
}

EvaluationResult EvalVisitor::visit(const Nodes::TimeShiftNode* node)
{
    const auto ret = dispatch(node->left());
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto timeShift = static_cast<int>(dispatch(node->right()).valueAsDouble());
    return ret.timeShift(timeShift);
}

EvaluationResult EvalVisitor::visit(const Nodes::TimeIndexNode* node)
{
    const auto ret = dispatch(node->left());
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto timeIndex = static_cast<int>(dispatch(node->right()).valueAsDouble());
    return ret[timeIndex];
}

EvaluationResult EvalVisitor::visit(const Nodes::TimeSumNode* node)
{
    const auto expression = dispatch(node->expression());
    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto from = static_cast<int>(dispatch(node->from()).valueAsDouble());

    // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue],
    const auto to = static_cast<int>(dispatch(node->to()).valueAsDouble());
    return expression.timeSum(from, to);
}

EvaluationResult EvalVisitor::visit(const Nodes::AllTimeSumNode* node)
{
    const EvaluationResult expression = dispatch(node->child());
    return expression.alltimeSum(fillContext_.getLocalNumberOfTimeSteps());
}

EvaluationResult EvalVisitor::visitDual(const Nodes::FunctionNode* node)
{
    const auto indexNode = dynamic_cast<Nodes::LiteralNode*>(node->getOperands().at(1));
    unsigned int cstrIndex = static_cast<unsigned int>(indexNode->value());
    const auto& variability = optimContainer_.getConstraintVariability(component_, cstrIndex);

    if (isTimeConstant(variability))
    {
        const auto constraints = optimContainer_.componentConstraints(component_, cstrIndex, 1);
        return EvaluationResult(constraints[0]->dual());
    }

    // The constraint depends on time
    const unsigned nbTimeStep = fillContext_.getLocalNumberOfTimeSteps();
    std::vector<double> constraintValues(nbTimeStep, 0.0);
    const auto componentConstraints = optimContainer_.componentConstraints(component_,
                                                                           cstrIndex,
                                                                           nbTimeStep);
    for (unsigned constraintInd = 0; constraintInd < nbTimeStep; ++constraintInd)
    {
        constraintValues[constraintInd] = componentConstraints[constraintInd]->dual();
    }

    return EvaluationResult{constraintValues};
}

auto PowerOp = [](const auto& a, const auto& b) { return std::pow(a, b); };
auto FloorOp = [](double d) { return std::floor(d); };
auto CeilOp = [](double d) { return std::ceil(d); };

EvaluationResult EvalVisitor::visitPow(const Nodes::FunctionNode* node)
{
    const auto numbers = node->getOperands();
    auto base = dispatch(numbers.at(0));
    auto exponent = dispatch(numbers.at(1));
    return base.evaluateBinaryOperation(exponent, PowerOp);
}

EvaluationResult EvalVisitor::visitFloor(const Nodes::FunctionNode* node)
{
    auto* floor_arg = node->getOperands()[0];
    return dispatch(floor_arg).evaluateUnaryOperation(FloorOp);
}

EvaluationResult EvalVisitor::visitCeil(const Nodes::FunctionNode* node)
{
    auto* ceil_arg = node->getOperands()[0];
    return dispatch(ceil_arg).evaluateUnaryOperation(CeilOp);
}

EvaluationResult EvalVisitor::visit(const Nodes::FunctionNode* node)
{
    switch (node->type())
    {
    case Nodes::FunctionNodeType::reduced_cost:
        return visitReducedCost(node);
    case Nodes::FunctionNodeType::dual:
        return visitDual(node);
    case Nodes::FunctionNodeType::max:
        return applyOperation(visitChildrenNodes(node),
                              [](const auto& v) { return *std::ranges::max_element(v); });
    case Nodes::FunctionNodeType::min:
        return applyOperation(visitChildrenNodes(node),
                              [](const auto& v) { return *std::ranges::min_element(v); });
    case Nodes::FunctionNodeType::pow:
        return visitPow(node);
    case Nodes::FunctionNodeType::floor:
        return visitFloor(node);
    case Nodes::FunctionNodeType::ceil:
        return visitCeil(node);
    default:
        return EvaluationResult(0);
    }
}

EvaluationResult EvalVisitor::visitReducedCost(const Nodes::FunctionNode* node)
{
    const auto varNode = dynamic_cast<Nodes::VariableNode*>(node->getOperands().at(0));

    if (isTimeConstant(varNode->variability()))
    {
        const std::span componentVariables = optimContainer_.getComponentVariable(
          component_,
          varNode->Index(),
          1 /* single timestep*/);
        return EvaluationResult(componentVariables[0]->reducedCost());
    }

    // The variable depends on time
    const unsigned nbTimeStep = fillContext_.getLocalNumberOfTimeSteps();
    std::vector<double> varValues(nbTimeStep, 0.);
    const std::span componentVariables = optimContainer_.getComponentVariable(component_,
                                                                              varNode->Index(),
                                                                              nbTimeStep);
    for (unsigned varInd = 0; varInd < nbTimeStep; ++varInd)
    {
        varValues[varInd] = componentVariables[varInd]->reducedCost();
    }
    return EvaluationResult{varValues};
}

std::string EvalVisitor::name() const
{
    return "EvalVisitor";
}

EvalVisitorDivisionException::EvalVisitorDivisionException(double left,
                                                           double right,
                                                           const std::string& message):
    std::runtime_error("DivisionNode: Error while evaluating : " + std::to_string(left) + "/"
                       + std::to_string(right) + " " + message)
{
}

EvalVisitorNotImplemented::EvalVisitorNotImplemented(const std::string& visitor,
                                                     const std::string& node):
    std::invalid_argument("Visitor" + visitor + " not implemented for node type " + node)
{
}

EvaluationResult::EvaluationResult(double value):
    value_(value)
{
}

EvaluationResult::EvaluationResult(const std::vector<double>& values):
    value_(values)
{
}

size_t EvaluationResult::size() const
{
    if (std::holds_alternative<std::vector<double>>(value_))
    {
        return std::get<std::vector<double>>(value_).size();
    }
    return 1;
}

double EvaluationResult::value(unsigned i) const
{
    if (std::holds_alternative<std::vector<double>>(value_))
    {
        const auto& values = std::get<std::vector<double>>(value_);
        return values.at(i);
    }
    return std::get<double>(value_);
}

EvaluationResult::EvaluationResult(const std::variant<double, std::vector<double>>& value):
    value_(value)
{
}

double shift(double value, int)
{
    return value;
}

std::vector<double> shift(const std::vector<double>& values, int timeShift)
{
    return shiftVector(values, timeShift);
}

EvaluationResult EvaluationResult::timeShift(int time_shift) const
{
    return EvaluationResult(
      std::visit([&time_shift](const auto& l) -> std::variant<double, std::vector<double>>
                 { return shift(l, time_shift); },
                 value_));
}

EvaluationResult EvaluationResult::timeSum(int from, int to) const
{
    EvaluationResult ret(0.);
    for (int shift = from; shift <= to; ++shift)
    {
        ret += timeShift(shift);
    }
    return ret;
}

EvaluationResult EvaluationResult::alltimeSum(int numberOfTimeStep) const
{
    EvaluationResult ret(0.);
    for (auto t = 0; t < numberOfTimeStep; ++t)
    {
        ret += operator[](t);
    }
    return ret;
}

EvaluationResult EvaluationResult::operator[](int timeIndex) const
{
    if (std::holds_alternative<double>(value_))
    {
        return *this;
    }
    const auto& vec = std::get<std::vector<double>>(value_);
    if (timeIndex < 0 || (unsigned)timeIndex >= vec.size())
    {
        throw EvalResultTimeIndexOutOfRange("timeIndex is out of range");
    }
    return EvaluationResult(vec.at(timeIndex));
}

} // namespace Antares::Expressions::Visitors
