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
#include <map>
#include <ranges>
#include <utility>

#include <antares/solver/optim-model-filler/TimeDependentLinearExpression.h>
#include "antares/expressions/RotateIndex.h"

namespace Antares::Optimization
{

TimeDependentLinearExpression::TimeDependentLinearExpression(
  const Optimisation::LinearProblemApi::FillContext& fillContext,
  const LinearExpression& linearExpression):
    fillContext_(fillContext)
{
    for (auto timestep = fillContext.getLocalFirstTimeStep();
         timestep <= fillContext.getLocalLastTimeStep();
         ++timestep)
    {
        linearExpressions_.emplace(timestep, linearExpression);
    }
}

TimeDependentLinearExpression::TimeDependentLinearExpression(
  const Optimisation::LinearProblemApi::FillContext& fillContext):
    TimeDependentLinearExpression(fillContext, LinearExpression())
{
}

TimeDependentLinearExpression::TimeDependentLinearExpression(
  const Optimisation::LinearProblemApi::FillContext& fillContext,
  LinearExpressionMap linearExpressions):
    linearExpressions_(std::move(linearExpressions)),
    fillContext_(fillContext)
{
}

TimeDependentLinearExpression& TimeDependentLinearExpression::operator+=(
  const TimeDependentLinearExpression& other)
{
    add_maps(linearExpressions_, other.GetLinearExpressions());
    return *this;
}

TimeDependentLinearExpression& TimeDependentLinearExpression::operator-=(
  const TimeDependentLinearExpression& other)
{
    add_maps(linearExpressions_, other.GetLinearExpressions(), std::negate<>());
    return *this;
}

TimeDependentLinearExpression TimeDependentLinearExpression::operator+(
  const TimeDependentLinearExpression& other) const
{
    auto result(*this);
    result += other;
    return result;
}

TimeDependentLinearExpression TimeDependentLinearExpression::operator-(
  const TimeDependentLinearExpression& other) const
{
    auto result(*this);
    result -= other;
    return result;
}

template<typename BinaryOperator>
TimeDependentLinearExpression BinaryOpLinearExpression(
  const LinearExpressionMap& left,
  const LinearExpressionMap& right,
  BinaryOperator op,
  const Optimisation::LinearProblemApi::FillContext& fillContext)
{
    auto result(left);
    for (const auto& [timeStep, other_linear_expression]: right)
    {
        if (auto it = result.find(timeStep); it == result.end())
        {
            result.emplace(timeStep, other_linear_expression);
        }
        else
        {
            it->second = op(it->second, other_linear_expression);
        }
    }
    return TimeDependentLinearExpression(fillContext, std::move(result));
}

TimeDependentLinearExpression TimeDependentLinearExpression::operator*(
  const TimeDependentLinearExpression& other) const
{
    return BinaryOpLinearExpression(GetLinearExpressions(),
                                    other.GetLinearExpressions(),
                                    std::multiplies<>(),
                                    fillContext_);
}

TimeDependentLinearExpression TimeDependentLinearExpression::operator/(
  const TimeDependentLinearExpression& other) const
{
    return BinaryOpLinearExpression(GetLinearExpressions(),
                                    other.GetLinearExpressions(),
                                    std::divides<>(),
                                    fillContext_);
}

TimeDependentLinearExpression TimeDependentLinearExpression::operator-() const
{
    const auto& linearExpressions = GetLinearExpressions();
    LinearExpressionMap result;
    for (const auto& [timeStep, linearExpression]: linearExpressions)
    {
        result.emplace(timeStep, -linearExpression);
    }
    return TimeDependentLinearExpression(fillContext_, std::move(result));
}

const LinearExpressionMap& TimeDependentLinearExpression::GetLinearExpressions() const
{
    return linearExpressions_;
}

size_t TimeDependentLinearExpression::getSize() const
{
    return linearExpressions_.size();
}

TimeDependentLinearExpression TimeDependentLinearExpression::shiftLinearExpressions(
  int shiftValue) const
{
    LinearExpressionMap linearExpressions;
    for (const auto& timeStep: linearExpressions_ | std::views::keys)
    {
        linearExpressions.emplace(timeStep,
                                  linearExpressions_.at(
                                    rotatedIndex(timeStep, shiftValue, fillContext_)));
    }
    return TimeDependentLinearExpression(fillContext_, std::move(linearExpressions));
}

TimeDependentLinearExpression TimeDependentLinearExpression::operator[](int timeStep) const
{
    return TimeDependentLinearExpression(fillContext_, linearExpressions_.at(timeStep));
}

TimeDependentLinearExpression TimeDependentLinearExpression::timeSumLinearExpressions(int from,
                                                                                      int to) const
{
    TimeDependentLinearExpression ret(fillContext_);

    for (auto shift = from; shift <= to; ++shift)
    {
        ret += shiftLinearExpressions(shift);
    }
    return ret;
}

TimeDependentLinearExpression TimeDependentLinearExpression::allTimeSumLinearExpressions() const
{
    LinearExpression sum;
    for (const auto& expr: linearExpressions_ | std::views::values)
    {
        sum += expr;
    }
    return TimeDependentLinearExpression(fillContext_, sum);
}

} // namespace Antares::Optimization
