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
#include <stdexcept>

#include <antares/solver/optim-model-filler/TimeDependentLinearExpression.h>

namespace Antares::Optimization
{

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
  const std::unordered_map<unsigned int, LinearExpression>& linearExpressions):
    linearExpressions_(linearExpressions)

{
}

TimeDependentLinearExpression TimeDependentLinearExpression::operator+(
  const TimeDependentLinearExpression& other) const
{
    return TimeDependentLinearExpression(
      add_maps(GetLinearExpressions(), other.GetLinearExpressions()));
}

TimeDependentLinearExpression TimeDependentLinearExpression::operator-(
  const TimeDependentLinearExpression& other) const
{
    return TimeDependentLinearExpression(
      add_maps(GetLinearExpressions(), other.GetLinearExpressions(), std::negate<>()));
}

template<typename BinaryOperator>
TimeDependentLinearExpression BinaryOpLinearExpression(
  const std::unordered_map<unsigned int, LinearExpression>& left,
  const std::unordered_map<unsigned int, LinearExpression>& right,
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
    return TimeDependentLinearExpression(std::move(result));
}

TimeDependentLinearExpression TimeDependentLinearExpression::operator*(
  const TimeDependentLinearExpression& other) const
{
    return BinaryOpLinearExpression(GetLinearExpressions(),
                                    other.GetLinearExpressions(),
                                    std::multiplies<>());
}

TimeDependentLinearExpression TimeDependentLinearExpression::operator/(
  const TimeDependentLinearExpression& other) const
{
    return BinaryOpLinearExpression(GetLinearExpressions(),
                                    other.GetLinearExpressions(),
                                    std::divides<>());
}

TimeDependentLinearExpression TimeDependentLinearExpression::operator-() const
{
    const auto& linear_expressions = GetLinearExpressions();
    std::unordered_map<unsigned int, LinearExpression> result;
    for (size_t i = 0; i < linear_expressions.size(); ++i)
    {
        result[i] = -linear_expressions.at(i);
    }
    return TimeDependentLinearExpression(std::move(result));
}

const std::unordered_map<unsigned int, LinearExpression>&
TimeDependentLinearExpression::GetLinearExpressions() const
{
    return linearExpressions_;
}

size_t TimeDependentLinearExpression::getSize() const
{
    return linearExpressions_.size();
}

Optimisation::LinearProblemApi::FillContext TimeDependentLinearExpression::DeduceFillContext() const
{
    // TODO it would be much simpler if all ctor have fillContex
    std::map ordered(linearExpressions_.begin(), linearExpressions_.end());
    return {ordered.begin()->first, ordered.rbegin()->first};
}

int rotatedIndex(unsigned key,
                 int shift,
                 const Optimisation::LinearProblemApi::FillContext& fillContext)
{
    unsigned rangeSize = fillContext.getNumberOfTimestep();

    // Normalize shift within bounds (to prevent negative indexing)
    shift = (shift % static_cast<int>(rangeSize) + rangeSize) % static_cast<int>(rangeSize);

    // Compute which key's value should be assigned to `key`
    return fillContext.getFirstTimeStep()
           + (key - fillContext.getFirstTimeStep() + shift) % rangeSize;
}

TimeDependentLinearExpression TimeDependentLinearExpression::shiftLinearExpressions(
  int shiftValue) const
{
    auto fillContext = DeduceFillContext();
    unsigned int number_of_timestep = fillContext.getNumberOfTimestep();

    if (number_of_timestep == 0)
    {
        return TimeDependentLinearExpression(fillContext);
    }

    std::unordered_map<unsigned int, LinearExpression> linearExpressions;
    for (const auto& timeStep: linearExpressions_ | std::views::keys)
    {
        linearExpressions[timeStep] = linearExpressions_.at(
          rotatedIndex(timeStep, shiftValue, fillContext));
    }
    return TimeDependentLinearExpression(std::move(linearExpressions));
}

TimeDependentLinearExpression TimeDependentLinearExpression::operator[](int index) const
{
    // TODO check?
    return TimeDependentLinearExpression({{index, linearExpressions_.at(index)}});
}

} // namespace Antares::Optimization
