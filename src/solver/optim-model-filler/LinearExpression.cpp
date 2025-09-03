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
/**
 * Element-wise multiplication of a map by a scale.
 * For every key: final_value = scale * initial_value
 * @param map The [string, double] map to scale
 * @param scale The scale
 * @return The scaled map
 */
FullKeyMap scale_map(const FullKeyMap& map, double scale)
{
    FullKeyMap result;
    for (auto [key, value]: map)
    {
        result[key] = scale * value;
    }
    return result;
}

LinearExpression::LinearExpression():
    offset_(0.),
    coef_per_var_({})
{
}

LinearExpression::LinearExpression(
  const Expressions::Visitors::EvaluationResult& offset,
  const std::vector<Expressions::Visitors::EvaluationResult>& coef_per_var):
    offset_(offset),
    coef_per_var_(coef_per_var)
{
}

LinearExpression LinearExpression::operator+(const LinearExpression& other) const
{
    auto result(*this);
    result += other;
    return result;
}

LinearExpression& LinearExpression::operator+=(const LinearExpression& other)
{
    this->offset_ += other.offset_;
    for (auto i = 0; i < coef_per_var_.size(); ++i)
    {
        coef_per_var_[i] += other.coef_per_var_.at(i);
    }
    return *this;
}

LinearExpression LinearExpression::ShiftLinearExpressions(int timeShift) const
{
    std::vector shiftedCoefPerVar(coef_per_var_.size(), Expressions::Visitors::EvaluationResult{0});
    for (auto i(0); i < coef_per_var_.size(); ++i)
    {
        shiftedCoefPerVar[i] = coef_per_var_.at(i).timeShift(timeShift);
    }

    return {std::move(offset_.timeShift(timeShift)), std::move(shiftedCoefPerVar)};
}

LinearExpression LinearExpression::operator[](int timeIndex) const
{
    std::vector coefPerVar(coef_per_var_.size(), Expressions::Visitors::EvaluationResult{0});
    for (auto i(0); i < coef_per_var_.size(); ++i)
    {
        coefPerVar[i] = coef_per_var_.at(i)[timeIndex];
    }
    return {std::move(offset_[timeIndex]), std::move(coefPerVar)};
}

LinearExpression LinearExpression::TimeSumLinearExpressions(int from, int to) const
{
    std::vector coefPerVar(coef_per_var_.size(), Expressions::Visitors::EvaluationResult{0});
    for (auto i(0); i < coef_per_var_.size(); ++i)
    {
        coefPerVar[i] = coef_per_var_.at(i).timeSum(from, to);
    }
    return {std::move(offset_.timeSum(from, to)), std::move(coefPerVar)};
}

LinearExpression LinearExpression::AllTimeSumLinearExpressions(unsigned int nbTimeStep) const
{
    std::vector coefPerVar(coef_per_var_.size(), Expressions::Visitors::EvaluationResult{0});
    for (auto i(0); i < coef_per_var_.size(); ++i)
    {
        coefPerVar[i] = coef_per_var_.at(i).alltimeSum(nbTimeStep);
    }
    return {std::move(offset_.alltimeSum(nbTimeStep)), std::move(coefPerVar)};
}

const std::vector<Expressions::Visitors::EvaluationResult>& LinearExpression::coefPerVar() const
{
    return coef_per_var_;
}

LinearExpression LinearExpression::operator-(const LinearExpression& other) const
{
    auto result(*this);
    result += -other;
    return result;
}

LinearExpression& LinearExpression::operator-=(const LinearExpression& other)
{
    return *this += -other;
}

std::vector<Expressions::Visitors::EvaluationResult> operator*(
  const std::vector<Expressions::Visitors::EvaluationResult>& rights,
  const Expressions::Visitors::EvaluationResult& scale)
{
    auto ret(rights);
    for (auto& v: ret)
    {
        v *= scale;
    }
    return ret;
}

std::vector<Expressions::Visitors::EvaluationResult>& operator*=(
  std::vector<Expressions::Visitors::EvaluationResult>& rights,
  const Expressions::Visitors::EvaluationResult& scale)
{
    for (auto& right: rights)
    {
        right *= scale;
    }
    return rights;
}

LinearExpression LinearExpression::operator*(const LinearExpression& other) const
{
    bool localCoeffPerVarIsEmpty = std::ranges::all_of(
      coef_per_var_,
      [&](const Expressions::Visitors::EvaluationResult& coefficients)
      { return coefficients.IsEmptyOrZero(); });
    bool otherCoeffPerVarIsEmpty = std::ranges::all_of(
      other.coef_per_var_,
      [&](const Expressions::Visitors::EvaluationResult& coefficients)
      { return coefficients.IsEmptyOrZero(); });
    if (localCoeffPerVarIsEmpty)
    {
        return {offset_ * other.offset_, other.coef_per_var_ * offset_};
    }
    else if (otherCoeffPerVarIsEmpty)
    {
        return {offset_ * other.offset_, coef_per_var_ * other.offset_};
    }
    else
    {
        throw std::invalid_argument("A linear expression can't have quadratic terms.");
    }
}
LinearExpression& LinearExpression::operator*=(const LinearExpression& other)
{
    bool localCoeffPerVarIsEmpty = std::ranges::all_of(
      coef_per_var_,
      [&](const Expressions::Visitors::EvaluationResult& coefficients)
      { return coefficients.IsEmptyOrZero(); });

    bool otherCoeffPerVarIsEmpty = std::ranges::all_of(
      other.coef_per_var_,
      [&](const Expressions::Visitors::EvaluationResult& coefficients)
      { return coefficients.IsEmptyOrZero(); });

    if (localCoeffPerVarIsEmpty)
    {
        coef_per_var_ = other.coef_per_var_ * offset_;
        offset_ *= other.offset_;
        return *this;
    }
    else if (otherCoeffPerVarIsEmpty)
    {
        offset_ *= other.offset_;
        coef_per_var_ *= other.offset_;
        return *this;
    }
    else
    {
        throw std::invalid_argument("A linear expression can't have quadratic terms.");
    }
}

LinearExpression LinearExpression::operator/(const LinearExpression& other) const
{
    if (!std::ranges::all_of(other.coef_per_var_,
                             [&](const Expressions::Visitors::EvaluationResult& coefficients)
                             { return coefficients.IsEmptyOrZero(); }))
    {
        throw std::invalid_argument("A linear expression can't have a variable as a divisor.");
    }
    return {offset_ / other.offset_,
            coef_per_var_ * (Expressions::Visitors::EvaluationResult(1.0) / other.offset_)};
}
LinearExpression& LinearExpression::operator/=(const LinearExpression& other)
{
    if (!std::ranges::all_of(other.coef_per_var_,
                             [&](const Expressions::Visitors::EvaluationResult& coefficients)
                             { return coefficients.IsEmptyOrZero(); }))
    {
        throw std::invalid_argument("A linear expression can't have a variable as a divisor.");
    }
    offset_ /= other.offset_;
    coef_per_var_ *= (Expressions::Visitors::EvaluationResult(1.0) / other.offset_);
    return *this;
}

LinearExpression LinearExpression::operator-() const
{
    return {-offset_, coef_per_var_ * Expressions::Visitors::EvaluationResult(-1)};
}

const Expressions::Visitors::EvaluationResult& LinearExpression::offset() const
{
    return offset_;
}

} // namespace Antares::Optimization
