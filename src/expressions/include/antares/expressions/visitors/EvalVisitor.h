/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include <cmath>
#include <variant>

#include <antares/expressions/visitors/EvaluationContext.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include "antares/expressions/visitors/NodeVisitor.h"

namespace Antares::Optimisation::LinearProblemApi
{
struct DataSeriesKeys;
}

namespace Antares::Expressions::Visitors
{

class EvalVisitorDivisionException: public std::runtime_error
{
public:
    EvalVisitorDivisionException(double left, double right, const std::string& message);
};

class EvalVisitorNotImplemented: public std::invalid_argument
{
public:
    EvalVisitorNotImplemented(const std::string& visitor, const std::string& node);
};

class EvaluationResult
{
public:
    explicit EvaluationResult(double value);

    explicit EvaluationResult(const std::vector<double>& values);

    EvaluationResult operator+(const EvaluationResult& right) const
    {
        return applyOperator(right, std::plus<>());
    }

    EvaluationResult operator-(const EvaluationResult& right) const
    {
        return applyOperator(right, std::minus<>());
    }

    EvaluationResult operator*(const EvaluationResult& right) const
    {
        return applyOperator(right, std::multiplies<>());
    }

    struct SafeDivides
    {
        double operator()(double lhs, double rhs) const
        {
            // if (rhs == 0.0)
            // {
            //     throw std::runtime_error("Division by zero in EvaluationResult.");
            // }
            double result{0.};
            try
            {
                result = lhs / rhs;

                if (!std::isfinite(result))
                {
                    throw EvalVisitorDivisionException(lhs, rhs, "is not a finite number");
                }
            }
            catch (const std::exception& ex)
            {
                throw EvalVisitorDivisionException(lhs, rhs, ex.what());
            }

            return result;
        }
    };

    EvaluationResult operator/(const EvaluationResult& right) const
    {
        // return applyOperator(right, std::divides<>());
        return applyOperator(right, SafeDivides{});
    }

    EvaluationResult operator-() const
    {
        return applyUnaryOperator(std::negate<>());
        // return applyOperator(EvaluationResult{-1}, std::multiplies<>());
    }

    [[nodiscard]] std::variant<double, std::vector<double>> value() const
    {
        return value_;
    }

    [[nodiscard]] double valueAsDouble() const
    {
        return std::get<double>(value_);
    }

    [[nodiscard]] std::vector<double> valuesAsVector() const
    {
        return std::get<std::vector<double>>(value_);
    }


private:
    std::variant<double, std::vector<double>> value_;

    template<typename Op>
    EvaluationResult applyOperator(const EvaluationResult& right, Op op) const;
    template<typename Op>
    EvaluationResult applyUnaryOperator(Op op) const;
};

template<typename BinaryOp>
double applyOperation(double lhs, double rhs, BinaryOp op)
{
    return op(lhs, rhs);
}

template<typename BinaryOp>
std::vector<double> applyOperation(const std::vector<double>& lhs, double rhs, BinaryOp op)
{
    auto result(lhs);
    for (double& value: result)
    {
        op(value, rhs);
    }
    return result;
}

template<typename BinaryOp>
std::vector<double> applyOperation(double lhs, const std::vector<double>& rhs, BinaryOp op)
{
    auto result(rhs);
    for (double& value: result)
    {
        op(value, lhs);
    }
    return result;
}

template<typename BinaryOp>
std::vector<double> applyOperation(const std::vector<double>& lhs, const std::vector<double>& rhs, BinaryOp op)
{
    if (lhs.size() == rhs.size())
    {
        std::vector<double> result(rhs.size());
        for (size_t i = 0; i < rhs.size(); ++i)
        {
            result[i] = op(lhs[i], rhs[i]);
        }
    }
    else
    {
        // TODO
        throw std::runtime_error("Evaluation Visitor error....");
    }
}

template<typename Op>
EvaluationResult EvaluationResult::applyOperator(const EvaluationResult& right, Op op) const
{
    return
    {
        std::visit([&op](const auto& l, const auto& r) { return applyOperation(l, r, op); },
                   value_,
                   right.value_);
    }
}

template<typename UnaryOp>
std::vector<double> applyOperation(const std::vector<double>& values, UnaryOp op)
{
    auto result(values);
    for (double& v: result)
    {
        v = op(v);
    }
    return result;
}

template<typename UnaryOp>
double applyOperation(double value, UnaryOp op)
{
    return op(value);
}

template<typename Op>
EvaluationResult EvaluationResult::applyUnaryOperator(Op op) const
{
    return
    {
        std::visit([&op](const auto& v) { return applyOperation(v, op); }, value_);
    }
}

/**
 * @brief Represents a visitor for evaluating expressions within a given context.
 */
class EvalVisitor: public NodeVisitor<EvaluationResult>
{
public:
    /**
     * @brief Default constructor, creates an evaluation visitor with no context.
     */
    // EvalVisitor() = default; // No context (variables / parameters)

    /**
     * @brief Constructs an evaluation visitor with the specified context.
     *
     * @param context The evaluation context.
     * @param dataSeriesKeys
     */
    explicit EvalVisitor(EvaluationContext context,
                         Optimisation::LinearProblemApi::DataSeriesKeys dataSeriesKeys);
    std::string name() const override;

private:
    const EvaluationContext context_;
    Optimisation::LinearProblemApi::DataSeriesKeys dataSeriesKeys_;
    EvaluationResult visit(const Nodes::SumNode* node) override;
    EvaluationResult visit(const Nodes::SubtractionNode* node) override;
    EvaluationResult visit(const Nodes::MultiplicationNode* node) override;
    EvaluationResult visit(const Nodes::DivisionNode* node) override;
    EvaluationResult visit(const Nodes::EqualNode* node) override;
    EvaluationResult visit(const Nodes::LessThanOrEqualNode* node) override;
    EvaluationResult visit(const Nodes::GreaterThanOrEqualNode* node) override;
    EvaluationResult visit(const Nodes::NegationNode* node) override;
    EvaluationResult visit(const Nodes::VariableNode* node) override;
    EvaluationResult visit(const Nodes::ParameterNode* node) override;
    EvaluationResult visit(const Nodes::LiteralNode* node) override;
    EvaluationResult visit(const Nodes::PortFieldNode* node) override;
    EvaluationResult visit(const Nodes::PortFieldSumNode* node) override;
    EvaluationResult visit(const Nodes::ComponentVariableNode* node) override;
    EvaluationResult visit(const Nodes::ComponentParameterNode* node) override;
};
} // namespace Antares::Expressions::Visitors
