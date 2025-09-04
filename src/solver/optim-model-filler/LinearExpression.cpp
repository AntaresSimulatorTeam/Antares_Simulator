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
#include "antares/expressions/ShiftVector.h"
using namespace Antares::Expressions::Visitors;

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

LinearExpression::LinearExpression(const std::vector<double>& offset,
                                   const std::vector<std::vector<double>>& coef_per_var):
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
    Expressions::Visitors::computeBinaryOperation(offset_, other.offset_, std::plus<>());
    // this->offset_ += other.offset_;
    for (auto i = 0; i < coef_per_var_.size(); ++i)
    {
        // coef_per_var_[i] += other.coef_per_var_.at(i);
        Expressions::Visitors::computeBinaryOperation(coef_per_var_[i],
                                                      other.coef_per_var_[i],
                                                      std::plus<>());
    }
    return *this;
}

LinearExpression LinearExpression::ShiftLinearExpressions(int timeShift) const
{
    std::vector shiftedCoefPerVar(coef_per_var_.size(),
                                  std::vector<double>(coef_per_var_.at(0).size(), 0));
    for (auto i(0); i < coef_per_var_.size(); ++i)
    {
        shiftedCoefPerVar[i] = std::move(
          shiftVector(coef_per_var_.at(i), timeShift)); // coef_per_var_.at(i).timeShift(timeShift);
    }

    return {std::move(shiftVector(offset_, timeShift)), std::move(shiftedCoefPerVar)};
}

LinearExpression LinearExpression::operator[](int timeIndex) const
{
    // TODO initialisation
    std::vector coefPerVar(coef_per_var_.size(),
                           std::vector<double>(coef_per_var_.at(0).size(), 0));
    for (auto i(0); i < coef_per_var_.size(); ++i)
    {
        coefPerVar[i][timeIndex] = coef_per_var_.at(i)[timeIndex];
    }
    std::vector<double> offset(offset_.size(), 0);
    offset[timeIndex] = offset_[timeIndex];
    return {offset, coefPerVar};
}

LinearExpression LinearExpression::TimeSumLinearExpressions(int from, int to) const
{
    LinearExpression result;
    for (auto shift = from; shift <= to; ++shift)
    {
        result += ShiftLinearExpressions(shift);
    }
    return result;
}

LinearExpression LinearExpression::AllTimeSumLinearExpressions(unsigned int nbTimeStep) const
{
    LinearExpression result;
    for (auto timeIndex = 0; timeIndex < nbTimeStep; ++timeIndex)
    {
        result += operator[](timeIndex);
    }
    return result;
}

const std::vector<std::vector<double>>& LinearExpression::coefPerVar() const
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



LinearExpression LinearExpression::operator*(const LinearExpression& other) const
{
    if (coef_per_var_.empty())
    {
        return {computeBinaryOperation(offset_, other.offset_, std::multiplies<>()),
                computeBinaryOperation(other.coef_per_var_, offset_, std::multiplies<>())};
    }
    else if (other.coef_per_var_.empty())
    {
        return {computeBinaryOperation(offset_, other.offset_, std::multiplies<>()),
                computeBinaryOperation(coef_per_var_, other.offset_, std::multiplies<>())};
    }
    else
    {
        throw std::invalid_argument("A linear expression can't have quadratic terms.");
    }
}
LinearExpression& LinearExpression::operator*=(const LinearExpression& other)
{
    if (coef_per_var_.empty())
    {
        coef_per_var_ = computeBinaryOperation(other.coef_per_var_, offset_, std::multiplies<>());
        computeBinaryOperation(offset_, other.offset_, std::multiplies<>());
        return *this;
    }
    else if (other.coef_per_var_.empty())
    {
        computeBinaryOperation(offset_, other.offset_, std::multiplies<>());
        computeBinaryOperation(coef_per_var_, other.offset_, std::multiplies<>());
        return *this;
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
        throw std::invalid_argument("A linear expression can't have a variable as a divisor.");
    }
    return {computeBinaryOperation(offset_, other.offset_, std::divides<>()),
            computeBinaryOperation(coef_per_var_, other.offset_, std::divides<>())};
}
LinearExpression& LinearExpression::operator/=(const LinearExpression& other)
{
    if (!other.coef_per_var_.empty())
    {
        throw std::invalid_argument("A linear expression can't have a variable as a divisor.");
    }
    computeBinaryOperation(offset_, other.offset_, std::divides<>());
    computeBinaryOperation(coef_per_var_, other.offset_, std::divides<>());
    return *this;
}

LinearExpression LinearExpression::operator-() const
{
    return {computeBinaryOperation(offset_, -1, std::multiplies<>()),
            computeBinaryOperation(coef_per_var_, -1, std::multiplies<>())};
}

const std::vector<double>& LinearExpression::offset() const
{
    return offset_;
}

} // namespace Antares::Optimization
