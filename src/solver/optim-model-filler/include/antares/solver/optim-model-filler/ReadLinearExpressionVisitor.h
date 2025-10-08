/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once

#include <antares/expressions/visitors/NodeVisitor.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/solver/optim-model-filler/LinearExpression.h>
#include "antares/expressions/nodes/ExpressionsNodes.h"
#include "antares/expressions/visitors/EvalVisitor.h"
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/study/system-model/component.h"

/**
 * Read Linear Expression Visitor
 * Visits a Node and produces a Linear Expression (defined by an offset and non-zero
 * coefficients of variables)
 * Comparison Nodes are not allowed
 */
using namespace Antares::Expressions;

namespace Antares::Optimisation
{

class ReadLinearExpressionVisitor: public Expressions::Visitors::NodeVisitor<
                                     Antares::Optimization::TimeDependentLinearExpression>
{
public:
    /**
     * @brief Constructs a clone visitor with the specified registry for creating new nodes.
     *
     * @param registry The registry used for creating new nodes.
     */
    explicit ReadLinearExpressionVisitor(
      const OptimEntityContainer& optimEntityContainer,
      const Antares::ModelerStudy::SystemModel::Component& component,
      const Antares::Optimisation::LinearProblemApi::FillContext& fillContext):
        optimEntityContainer_(optimEntityContainer),
        component_(component),
        nbtimeSteps_(fillContext.getLocalNumberOfTimeSteps()),
        fillContext_(fillContext),
        evalContext_(optimEntityContainer.getEvaluationContext(component)),
        evalVisitor_(optimEntityContainer, fillContext, component)
    {
    }

    Antares::Optimization::TimeDependentLinearExpression visitRemoveDuplicates(
      const Nodes::Node* node)
    {
        auto expr = dispatch(node);
        expr.removeDuplicateCoefficients();
        return expr;
    }

    std::string name() const override
    {
        return "ReadLinearExpressionVisitor";
    }

    Antares::Optimization::TimeDependentLinearExpression visit(const Nodes::SumNode* node) override
    {
        const auto& operands = node->getOperands();
        Antares::Optimization::TimeDependentLinearExpression ret(nbtimeSteps_);
        for (auto* operand: operands)
        {
            ret += dispatch(operand);
        }
        return ret;
    }

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::SubtractionNode* node) override
    {
        auto ret = dispatch(node->left());
        ret -= dispatch(node->right());
        return ret;
    }

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::MultiplicationNode* node) override
    {
        auto ret = dispatch(node->left());
        ret *= dispatch(node->right());
        return ret;
    }

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::DivisionNode* node) override
    {
        throw std::runtime_error("Not implemented");
    }

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::EqualNode* node) override
    {
        throw std::runtime_error("Not implemented");
    }

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::LessThanOrEqualNode* node) override
    {
        throw std::runtime_error("Not implemented");
    }

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::GreaterThanOrEqualNode* node) override
    {
        throw std::runtime_error("Not implemented");
    }

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::NegationNode* node) override
    {
        auto ret = dispatch(node->child());
        return -ret;
    }

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::VariableNode* node) override
    {
        const auto variableStart = optimEntityContainer_.getVariableStartColumn(component_,
                                                                                node->Index());
        if (node->timeIndex() == Antares::Optimisation::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO)
        {
            Antares::Optimization::LinearExpression out;
            out.addVariable(variableStart, 1);
            return Antares::Optimization::TimeDependentLinearExpression(std::move(out));
        }
        if (node->timeIndex() == Antares::Optimisation::TimeIndex::VARYING_IN_TIME_ONLY
            || node->timeIndex()
                 == Antares::Optimisation::TimeIndex::VARYING_IN_TIME_AND_SCENARIO) /* scenario not
                                                                                  handled !*/
        {
            Antares::Optimization::TimeDependentLinearExpression out(nbtimeSteps_);

            auto variableIndex = variableStart;
            for (int ts = 0; ts < nbtimeSteps_; ts++)
            {
                out[ts].addVariable(variableIndex, 1);
                ++variableIndex;
            }
            return out;
        }
        throw "the support of scenario dependent variables is not available for now";
    }

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::ParameterNode* node) override
    {
        const auto systemParameter = evalContext_.getParameter(node->value());
        if (node->timeIndex() == Antares::Optimisation::TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO
            && systemParameter.type != Antares::ModelerStudy::SystemModel::ParameterType::CONSTANT)
        {
            throw std::runtime_error(
              "Parameter " + node->value()
              + " is declared constant in time and scenario in library but not in system");
        }

        if (systemParameter.type == Antares::ModelerStudy::SystemModel::ParameterType::CONSTANT)
        {
            double value = evalContext_.getSystemParameterValueAsDouble(node->value());
            return Antares::Optimization::TimeDependentLinearExpression(
              Antares::Optimization::LinearExpression({}, value));
        }
        // only dependent

        // assume global nb timeStep == nbtimeSteps
        const auto& parameters = evalContext_.getParameterValue(
          node->value(),
          fillContext_.getYear(),
          fillContext_.getGlobalFirstTimeStep() + fillContext_.getLocalFirstTimeStep(),
          fillContext_.getGlobalFirstTimeStep() + fillContext_.getLocalLastTimeStep());

        return Antares::Optimization::TimeDependentLinearExpression(parameters);
    }

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::LiteralNode* node) override
    {
        Antares::Optimization::LinearExpression ret({}, node->value()); // Constant expr
        return Antares::Optimization::TimeDependentLinearExpression(std::move(ret));
    }

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::PortFieldNode* node) override
    {
        throw std::runtime_error("Not implemented");
    }

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::PortFieldSumNode* node) override
    {
        auto& portId = node->getPortName();
        auto& fieldId = node->getFieldName();

        Antares::Optimization::TimeDependentLinearExpression to_return(nbtimeSteps_);

        for (const auto connexion_end: component_.componentConnectionsViaPort(portId))
        {
            auto* component = connexion_end.component();
            auto* port = connexion_end.port();

            ReadLinearExpressionVisitor visitor(optimEntityContainer_, *component, fillContext_);

            const Nodes::Node* node = component->nodeAtPortField(port->Id(), fieldId);
            to_return += visitor.dispatch(node);
        }

        return to_return;
    }

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::TimeShiftNode* node) override
    {
        auto expression = dispatch(node->left());
        if (expression.size() == 1)
        {
            return expression;
        }
        // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue]
        const auto timeIndex = static_cast<int>(
          evalVisitor_.dispatch(node->right()).valueAsDouble());
        expression.rotate(timeIndex);
        return expression;
    }

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::TimeIndexNode* node) override
    {
        auto expression = dispatch(node->left());

        if (expression.size() == 1)
        {
            return expression;
        }
        // it must be single value:  expression[IHaveTobeEvaluatedAsSingleValue]
        const auto timeIndex = static_cast<int>(
          evalVisitor_.dispatch(node->right()).valueAsDouble());
        return Antares::Optimization::TimeDependentLinearExpression(
          std::move(expression[timeIndex]));
    }

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::TimeSumNode* node) override
    {
        auto expression = dispatch(node->expression());

        const auto from = static_cast<int>(evalVisitor_.dispatch(node->from()).valueAsDouble());
        const auto to = static_cast<int>(evalVisitor_.dispatch(node->to()).valueAsDouble());

        if (expression.size() == 1)
        {
            // example from=0, to=2 => length({0, 1, 2}) = 3
            expression *= static_cast<double>(to - from + 1);
            return expression;
        }

        Antares::Optimization::TimeDependentLinearExpression ret(nbtimeSteps_);
        expression.rotate(from);
        for (int t = from; t <= to; ++t)
        {
            ret += expression;
            if (t < to)
            {
                expression.rotate(1);
            }
        }
        return ret;
    }

    Antares::Optimization::TimeDependentLinearExpression visit(
      const Nodes::AllTimeSumNode* node) override
    {
        Antares::Optimization::LinearExpression ret; // Constant expr
        auto expr = dispatch(node->child());
        for (auto& s: expr)
        {
            ret += s;
        }
        return Antares::Optimization::TimeDependentLinearExpression(std::move(ret));
    }

private:
    const Antares::Optimisation::OptimEntityContainer& optimEntityContainer_;
    const Antares::ModelerStudy::SystemModel::Component& component_;
    const Antares::Optimisation::EvaluationContext& evalContext_;
    const Antares::Optimisation::LinearProblemApi::FillContext& fillContext_;
    Antares::Expressions::Visitors::EvalVisitor evalVisitor_;
    const int nbtimeSteps_;
};
} // namespace Antares::Optimisation
