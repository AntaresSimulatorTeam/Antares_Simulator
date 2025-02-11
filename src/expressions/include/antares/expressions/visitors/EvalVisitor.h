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
    EvalVisitorDivisionException(const std::string& message);
};

class EvalVisitorNotImplemented: public std::invalid_argument
{
public:
    EvalVisitorNotImplemented(const std::string& visitor, const std::string& node);
};
enum class EvaluationResultType : bool
{
    CONSTANT = true,
    NOTCONSTANT = false
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

    EvaluationResult operator/(const EvaluationResult& right) const
    {
        return applyOperator(right, std::divides<>());
    }

    EvaluationResult operator-() const
    {
        return applyUnaryOperator(std::negate<>());
    }

    [[nodiscard]] double value() const
    {
        return value_;
    }

    [[nodiscard]] std::vector<double> values() const
    {
        return values_;
    }

    [[nodiscard]] EvaluationResultType getEvaluationResultType() const
    {
        return evaluationResultType;
    }

private:
    double value_ = 0.;
    std::vector<double> values_ = {};
    EvaluationResultType evaluationResultType;

    template<typename Op>
    EvaluationResult applyOperator(const EvaluationResult& right, Op op) const;
    template<typename Op>
    EvaluationResult applyUnaryOperator(Op op) const;
};

template<typename Op>
EvaluationResult EvaluationResult::applyOperator(const EvaluationResult& right, Op op) const
{
    EvaluationResult result(0.0);

    if (evaluationResultType == EvaluationResultType::CONSTANT
        && right.evaluationResultType == EvaluationResultType::CONSTANT)
    {
        result.value_ = op(value_, right.value_);
    }
    else if (evaluationResultType == EvaluationResultType::CONSTANT)
    {
        result.values_ = right.values_;
        for (double& v: result.values_)
        {
            v = op(value_, v);
        }
    }
    else if (right.evaluationResultType == EvaluationResultType::CONSTANT)
    {
        result.values_ = values_;
        for (double& v: result.values_)
        {
            v = op(v, right.value_);
        }
    }
    else if (values_.size() == right.values_.size())
    {
        result.values_ = values_;
        for (size_t i = 0; i < values_.size(); ++i)
        {
            result.values_[i] = op(values_[i], right.values_[i]);
        }
    }

    return result;
}

template<typename Op>
EvaluationResult EvaluationResult::applyUnaryOperator(Op op) const
{
    EvaluationResult result(0.0);

    if (evaluationResultType == EvaluationResultType::CONSTANT)
    {
        result.value_ = op(value_);
    }
    else
    {
        result.values_ = values_;
        for (double& v: result.values_)
        {
            v = op(v);
        }
    }

    return result;
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
    explicit EvalVisitor(EvaluationContext context, Optimisation::LinearProblemApi::DataSeriesKeys dataSeriesKeys);
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
