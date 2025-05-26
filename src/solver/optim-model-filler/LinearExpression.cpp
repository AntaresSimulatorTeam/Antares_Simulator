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

LinearExpression::LinearExpression(double offset, FullKeyMap coef_per_var):
    offset_(offset),
    coef_per_var_(std::move(coef_per_var))
{
}

LinearExpression LinearExpression::operator+(const LinearExpression& other) const
{
    return {offset_ + other.offset_, add_maps(coef_per_var_, other.coef_per_var_)};
}

const FullKeyMap& LinearExpression::coefPerVar() const
{
    return coef_per_var_;
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

double LinearExpression::offset() const
{
    return offset_;
}

} // namespace Antares::Optimization
