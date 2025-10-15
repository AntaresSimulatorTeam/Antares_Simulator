/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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
#include <functional>
#include <sstream>
#include <variant>

#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include "antares/expressions/visitors/NodeVisitor.h"
#include "antares/modeler-optimisation-container/EvaluationContext.h"
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/solver/optim-model-filler/Dimensions.h"
#include "antares/study/system-model/component.h"

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
        return evaluateBinaryOperation(right, std::plus<>());
    }

    EvaluationResult operator+=(const EvaluationResult& right)
    {
        *this = *this + right;
        return *this;
    }

    EvaluationResult operator-(const EvaluationResult& right) const
    {
        return evaluateBinaryOperation(right, std::minus<>());
    }

    EvaluationResult operator*(const EvaluationResult& right) const
    {
        return evaluateBinaryOperation(right, std::multiplies<>());
    }

    EvaluationResult operator==(const EvaluationResult& right) const
    {
        return evaluateBinaryOperation(right, std::equal_to<>());
    }

    EvaluationResult operator<=(const EvaluationResult& right) const
    {
        return evaluateBinaryOperation(right, std::less_equal<>());
    }

    EvaluationResult operator>=(const EvaluationResult& right) const
    {
        return evaluateBinaryOperation(right, std::greater_equal<>());
    }

    struct SafeDivides
    {
        static constexpr double DEFAULT_THRESHOLD = 1e-16;

        explicit SafeDivides(double threshold = DEFAULT_THRESHOLD):
            threshold_(threshold)
        {
        }

        double operator()(double lhs, double rhs) const
        {
            if (std::abs(rhs) <= threshold_)
            {
                throw EvalVisitorDivisionException(lhs, rhs, "Division by zero");
            }
            return lhs / rhs;
        }

    private:
        double threshold_;
    };

    EvaluationResult operator/(const EvaluationResult& right) const
    {
        return evaluateBinaryOperation(right, SafeDivides{});
    }

    EvaluationResult operator-() const
    {
        return evaluateUnaryOperation(std::negate<>());
    }

    [[nodiscard]] std::variant<double, std::vector<double>> value() const
    {
        return value_;
    }

    class EvalResultTypeError: public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    class EvalResultTimeIndexOutOfRange: public std::out_of_range
    {
    public:
        using std::out_of_range::out_of_range;
    };

    double valueAsDouble() const
    {
        if (!std::holds_alternative<double>(value_))
        {
            throw EvalResultTypeError("Expected a double but found a vector.");
        }
        return std::get<double>(value_);
    }

    [[nodiscard]] std::vector<double> valuesAsVector() const
    {
        if (const auto* v = std::get_if<std::vector<double>>(&value_))
        {
            return *v;
        }
        throw EvalResultTypeError("Expected a vector but found a double.");
    }

    [[nodiscard]] double getValueInVector(unsigned index) const
    {
        if (const auto* v = std::get_if<std::vector<double>>(&value_))
        {
            return (*v)[index];
        }
        throw EvalResultTypeError("Expected a vector but found a double.");
    }

    EvaluationResult operator[](int timeIndex) const;
    EvaluationResult timeShift(int time_shift) const;
    EvaluationResult timeSum(int from, int to) const;
    EvaluationResult alltimeSum(int numberOfTimeStep) const;

private:
    std::variant<double, std::vector<double>> value_;
    explicit EvaluationResult(const std::variant<double, std::vector<double>>& value);

    template<typename Op>
    EvaluationResult evaluateBinaryOperation(const EvaluationResult& right, Op op) const;
    template<typename Op>
    EvaluationResult evaluateUnaryOperation(Op op) const;

    static double shift(double value, int)
    {
        return value;
    }

    static std::vector<double> shift(const std::vector<double>& values, int shiftValue);
};

template<typename BinaryOp>
double computeBinaryOperation(double lhs, double rhs, BinaryOp op)
{
    return op(lhs, rhs);
}

template<typename BinaryOp>
std::vector<double> computeBinaryOperation(const std::vector<double>& lhs, double rhs, BinaryOp op)
{
    auto result(lhs);
    for (double& value: result)
    {
        value = op(value, rhs);
    }
    return result;
}

template<typename BinaryOp>
std::vector<double> computeBinaryOperation(double lhs, const std::vector<double>& rhs, BinaryOp op)
{
    std::vector<double> result(rhs.size());
    for (size_t i = 0; i < rhs.size(); ++i)
    {
        result[i] = op(lhs, rhs[i]);
    }
    return result;
}

class VectorsMismatchSize final: public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

template<typename BinaryOp>
std::vector<double> computeBinaryOperation(const std::vector<double>& lhs,
                                           const std::vector<double>& rhs,
                                           BinaryOp op)
{
    if (lhs.size() != rhs.size())
    {
        std::ostringstream errorMsg;
        errorMsg << "Failed to compute binary operation: vectors have different sizes ("
                 << lhs.size() << " and " << rhs.size() << ").";
        throw VectorsMismatchSize(errorMsg.str());
    }

    std::vector<double> result(lhs.size());
    for (size_t i = 0; i < lhs.size(); ++i)
    {
        result[i] = op(lhs[i], rhs[i]);
    }
    return result;
}

template<typename Op>
EvaluationResult EvaluationResult::evaluateBinaryOperation(const EvaluationResult& right,
                                                           Op op) const
{
    return EvaluationResult(
      std::visit([&op](const auto& l, const auto& r) -> std::variant<double, std::vector<double>>
                 { return computeBinaryOperation(l, r, op); },
                 value_,
                 right.value_));
}

template<typename UnaryOp>
std::vector<double> computeUnaryOperation(const std::vector<double>& values, UnaryOp op)
{
    auto result(values);
    for (double& v: result)
    {
        v = op(v);
    }
    return result;
}

template<typename UnaryOp>
double computeUnaryOperation(double value, UnaryOp op)
{
    return op(value);
}

template<typename Op>
EvaluationResult EvaluationResult::evaluateUnaryOperation(Op op) const
{
    return EvaluationResult(
      std::visit([&op](const auto& v) -> std::variant<double, std::vector<double>>
                 { return computeUnaryOperation(v, op); },
                 value_));
}

/**
 * @brief Represents a visitor for evaluating expressions within a given context.
 */
class EvalVisitor: public NodeVisitor<EvaluationResult>
{
public:
    /**
     * @brief Constructs an evaluation visitor with the specified context.
     *
     * @param context The evaluation context.
     * @param fillContext
     */

    explicit EvalVisitor(const Optimisation::OptimEntityContainer& optimContainer,
                         const Optimisation::LinearProblemApi::FillContext& fillContext,
                         const ModelerStudy::SystemModel::Component& component);

    std::string name() const override;

private:
    const Optimisation::EvaluationContext& context_;
    const Optimisation::LinearProblemApi::FillContext& fillContext_;
    const ModelerStudy::SystemModel::Component& component_;
    const Optimisation::OptimEntityContainer& optimContainer_;

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
    EvaluationResult visit(const Nodes::TimeShiftNode* node) override;
    EvaluationResult visit(const Nodes::TimeIndexNode* node) override;
    EvaluationResult visit(const Nodes::TimeSumNode* node) override;
    EvaluationResult visit(const Nodes::AllTimeSumNode* node) override;
};
} // namespace Antares::Expressions::Visitors
