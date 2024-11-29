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

#include <stdexcept>

#include <antares/solver/optim-model-filler/LinearExpression.h>

namespace Antares::Optimization
{
static std::map<std::string, double> add_maps(std::map<std::string, double> left,
                                              std::map<std::string, double> right,
                                              double rhs_multiplier)
{
    std::map result(left);
    for (auto [var_id, coef]: right)
    {
        if (result.contains(var_id))
        {
            result[var_id] += rhs_multiplier * coef;
        }
        else
        {
            result[var_id] = rhs_multiplier * coef;
        }
    }
    return result;
}

static std::map<std::string, double> scale_map(std::map<std::string, double> map, double scale)
{
    std::map<std::string, double> result;
    for (auto [var_id, coef]: map)
    {
        result[var_id] = scale * coef;
    }
    return result;
}

LinearExpression::LinearExpression()
{
}

LinearExpression::LinearExpression(double scalar, std::map<std::string, double> coef_per_var):
    scalar_(scalar),
    coef_per_var_(std::move(coef_per_var))
{
}

LinearExpression LinearExpression::operator+(const LinearExpression& other) const
{
    return {scalar_ + other.scalar_, add_maps(coef_per_var_, other.coef_per_var_, 1)};
}

LinearExpression LinearExpression::operator-(const LinearExpression& other) const
{
    return {scalar_ - other.scalar_, add_maps(coef_per_var_, other.coef_per_var_, -1)};
}

LinearExpression LinearExpression::operator*(const LinearExpression& other) const
{
    if (coef_per_var_.empty())
    {
        return {scalar_ * other.scalar_, scale_map(other.coef_per_var_, scalar_)};
    }
    else if (other.coef_per_var_.empty())
    {
        return {scalar_ * other.scalar_, scale_map(coef_per_var_, other.scalar_)};
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
    return LinearExpression(scalar_ / other.scalar_, scale_map(coef_per_var_, 1 / other.scalar_));
}

LinearExpression LinearExpression::negate() const
{
    return {-scalar_, scale_map(coef_per_var_, -1)};
}
} // namespace Antares::Optimization
