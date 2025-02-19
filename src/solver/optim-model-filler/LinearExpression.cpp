/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
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

#include <algorithm>
#include <functional>
#include <stdexcept>

#include <antares/solver/optim-model-filler/LinearExpression.h>

namespace Antares::Optimization
{
template<typename T>
struct IdentityFunction
{
    const T& operator()(const T& t)
    {
        return t;
    }
};

/**
 * Element-wise sum of two [string, double] maps, preceded an element-wise multiplication of the
 * right-hand-side map. Keys that do not exist in one of the two maps are considered to have a zero
 * value. For every key: value = left_value + rhs_multiplier * right_value
 * @param left The left hand side map
 * @param right The right hand side map
 * @return The map resulting from the operation
 */
template<typename Key, typename Value, typename UnaryOp = IdentityFunction<Value>>
static std::map<Key, Value> add_maps(const std::map<Key, Value>& left,
                                     const std::map<Key, Value>& right,
                                     UnaryOp op = IdentityFunction<Value>{})
{
    std::map result(left);
    for (auto [key, value]: right)
    {
        if (result.contains(key))
        {
            result[key] += op(value);
        }
        else
        {
            result[key] = op(value);
        }
    }
    return result;
}

/**
 * Element-wise multiplication of a map by a scale.
 * For every key: final_value = scale * initial_value
 * @param map The [string, double] map to scale
 * @param scale The scale
 * @return The scaled map
 */
static std::map<std::string, double> scale_map(const std::map<std::string, double>& map,
                                               double scale)
{
    std::map<std::string, double> result;
    for (auto [key, value]: map)
    {
        result[key] = scale * value;
    }
    return result;
}

LinearExpression::LinearExpression(double offset, std::map<std::string, double> coef_per_var):
    offset_(offset),
    coef_per_var_(std::move(coef_per_var))
{
}

LinearExpression LinearExpression::operator+(const LinearExpression& other) const
{
    return {offset_ + other.offset_, add_maps(coef_per_var_, other.coef_per_var_)};
}

LinearExpression& LinearExpression::operator+=(const LinearExpression& other)
{
    this->offset_ += other.offset_;
    this->coef_per_var_ = add_maps(coef_per_var_, other.coef_per_var_);
    return *this;
}

LinearExpression LinearExpression::operator-(const LinearExpression& other) const
{
    return {offset_ - other.offset_,
            add_maps(coef_per_var_, other.coef_per_var_, std::negate<double>())};
}

LinearExpression LinearExpression::operator*(const LinearExpression& other) const
{
    if (coef_per_var_.empty())
    {
        return {offset_ * other.offset_, scale_map(other.coef_per_var_, offset_)};
    }
    else if (other.coef_per_var_.empty())
    {
        return {offset_ * other.offset_, scale_map(coef_per_var_, other.offset_)};
    }
    else
    {
        throw std::invalid_argument("A linear expression can't have quadratic terms.");
    }
}

LinearExpression LinearExpression::operator/(const LinearExpression& other) const
{
    if (!other.coef_per_var_.empty())
    {
        throw std::invalid_argument("A linear expression can't have a variable as a dividend.");
    }
    return LinearExpression(offset_ / other.offset_, scale_map(coef_per_var_, 1 / other.offset_));
}

LinearExpression LinearExpression::operator-() const
{
    return {-offset_, scale_map(coef_per_var_, -1)};
}

TimeDependentLinearExpression::TimeDependentLinearExpression(
  const Optimisation::LinearProblemApi::FillContext& fillContext,
  const LinearExpression& linearExpression)
{
    for (auto timestep(fillContext.getFirstTimeStep()); timestep <= fillContext.getLastTimeStep();
         ++timestep)
    {
        linearExpressions_[timestep] = linearExpression;
    }
}

TimeDependentLinearExpression::TimeDependentLinearExpression(
  const Optimisation::LinearProblemApi::FillContext& fillContext):
    TimeDependentLinearExpression(fillContext, LinearExpression())
{
}

TimeDependentLinearExpression::TimeDependentLinearExpression(
  const std::map<unsigned int, LinearExpression>& linearExpressions):
    linearExpressions_(linearExpressions)

{
}

TimeDependentLinearExpression TimeDependentLinearExpression::operator+(
  const TimeDependentLinearExpression& other) const
{
    // checkOtherLength(other);

    return {add_maps(GetLinearExpressions(), other.GetLinearExpressions())};
}

TimeDependentLinearExpression TimeDependentLinearExpression::operator-(
  const TimeDependentLinearExpression& other) const
{
    // checkOtherLength(other);

    return {add_maps(GetLinearExpressions(), other.GetLinearExpressions(), std::negate<>())};
}

template<typename BinaryOperator>
TimeDependentLinearExpression BinaryOpLinearExpression(
  const std::map<unsigned int, LinearExpression>& left,
  const std::map<unsigned int, LinearExpression>& right,
  BinaryOperator op)
{
    auto result(left);
    for (const auto& [timeStep, other_linear_expression]: right)
    {
        if (result.contains(timeStep))
        {
            result[timeStep] = op(result.at(timeStep), other_linear_expression);
        }
        else
        {
            result[timeStep] = other_linear_expression;
        }
    }
    return {std::move(result)};
}

TimeDependentLinearExpression TimeDependentLinearExpression::operator*(
  const TimeDependentLinearExpression& other) const
{
    //   checkOtherLength(other);
    return BinaryOpLinearExpression(GetLinearExpressions(),
                                    other.GetLinearExpressions(),
                                    std::multiplies<>());
}

TimeDependentLinearExpression TimeDependentLinearExpression::operator/(
  const TimeDependentLinearExpression& other) const
{
    // checkOtherLength(other);
    return BinaryOpLinearExpression(GetLinearExpressions(),
                                    other.GetLinearExpressions(),
                                    std::divides<>());
}

TimeDependentLinearExpression TimeDependentLinearExpression::operator-() const
{
    const auto& linear_expressions = GetLinearExpressions();
    std::map<unsigned int, LinearExpression> result;
    for (size_t i = 0; i < linear_expressions.size(); ++i)
    {
        result[i] = -linear_expressions.at(i);
    }
    return TimeDependentLinearExpression(std::move(result));
}

const std::map<unsigned int, LinearExpression>&
TimeDependentLinearExpression::GetLinearExpressions() const
{
    return linearExpressions_;
}

size_t TimeDependentLinearExpression::getSize() const
{
    return linearExpressions_.size();
}

} // namespace Antares::Optimization
