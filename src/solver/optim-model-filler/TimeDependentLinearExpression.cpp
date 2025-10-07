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
#include <stdexcept>

#include "antares/solver/optim-model-filler/LinearExpression.h"

namespace Antares::Optimization
{

TimeDependentLinearExpression::TimeDependentLinearExpression(std::size_t nbTimesteps)
{
    if (nbTimesteps == 1)
    {
        v_.emplace<LinearExpression>();
    }
    else
    {
        v_.emplace<std::vector<LinearExpression>>(nbTimesteps);
    }
}

TimeDependentLinearExpression::TimeDependentLinearExpression(const std::span<const double>& values)
{
    if (values.size() == 1)
    {
        v_.emplace<LinearExpression>(values[0]);
    }
    else
    {
        auto& out = v_.emplace<std::vector<LinearExpression>>();
        out.reserve(values.size());
        for (const double& v: values)
        {
            out.emplace_back(v);
        }
    }
}

TimeDependentLinearExpression::TimeDependentLinearExpression(LinearExpression&& expr):
    v_(std::move(expr))
{
}

void TimeDependentLinearExpression::expandTo(std::size_t nbTimesteps)
{
    if (auto* expr = std::get_if<LinearExpression>(&v_))
    {
        v_.emplace<std::vector<LinearExpression>>(nbTimesteps, *expr);
    }
}

std::vector<double> TimeDependentLinearExpression::constant() const
{
    if (auto* expr = std::get_if<LinearExpression>(&v_))
    {
        return {expr->constant()};
    }
    if (auto* expr = std::get_if<std::vector<LinearExpression>>(&v_))
    {
        std::vector<double> ret;
        ret.reserve(expr->size());
        for (const auto& x: *expr)
        {
            ret.push_back(x.constant());
        }
        return ret;
    }
    throw std::runtime_error("Invalid variant");
}

void TimeDependentLinearExpression::mergeDuplicateCoefficients()
{
    for (auto& expr: *this)
    {
        expr.mergeDuplicateCoefficients();
    }
}

LinearExpression* TimeDependentLinearExpression::begin()
{
    if (auto* expr = std::get_if<LinearExpression>(&v_))
    {
        return expr;
    }
    if (auto* expr = std::get_if<std::vector<LinearExpression>>(&v_))
    {
        return expr->data();
    }
    throw std::runtime_error("Invalid variant");
}

const LinearExpression* TimeDependentLinearExpression::begin() const
{
    if (const auto* expr = std::get_if<LinearExpression>(&v_))
    {
        return expr;
    }
    if (const auto* expr = std::get_if<std::vector<LinearExpression>>(&v_))
    {
        return expr->data();
    }
    throw std::runtime_error("Invalid variant");
}

LinearExpression* TimeDependentLinearExpression::end()
{
    if (auto* expr = std::get_if<LinearExpression>(&v_))
    {
        return expr + 1;
    }
    if (auto* expr = std::get_if<std::vector<LinearExpression>>(&v_))
    {
        return expr->data() + expr->size();
    }
    throw std::runtime_error("Invalid variant");
}

const LinearExpression* TimeDependentLinearExpression::end() const
{
    if (const auto* expr = std::get_if<LinearExpression>(&v_))
    {
        return expr + 1;
    }
    if (const auto* expr = std::get_if<std::vector<LinearExpression>>(&v_))
    {
        return expr->data() + expr->size();
    }
    throw std::runtime_error("Invalid variant");
}

LinearExpression& TimeDependentLinearExpression::operator[](std::size_t idx)
{
    if (auto* expr = std::get_if<LinearExpression>(&v_))
    {
        return *expr;
    }
    if (auto* expr = std::get_if<std::vector<LinearExpression>>(&v_))
    {
        return expr->operator[](idx);
    }
    throw std::runtime_error("Invalid variant");
}

const LinearExpression& TimeDependentLinearExpression::operator[](std::size_t idx) const
{
    if (const auto* expr = std::get_if<LinearExpression>(&v_))
    {
        return *expr;
    }
    if (const auto* expr = std::get_if<std::vector<LinearExpression>>(&v_))
    {
        return expr->operator[](idx);
    }
    throw std::runtime_error("Invalid variant");
}

std::size_t TimeDependentLinearExpression::size() const
{
    if (const auto* expr = std::get_if<LinearExpression>(&v_))
    {
        return 1;
    }
    if (const auto* expr = std::get_if<std::vector<LinearExpression>>(&v_))
    {
        return expr->size();
    }
    throw std::runtime_error("Invalid variant");
}

TimeDependentLinearExpression& TimeDependentLinearExpression::operator+=(
  const TimeDependentLinearExpression& other)
{
    if (other.size() > size())
    {
        expandTo(other.size());
    }
    for (std::size_t t = 0; t < size(); ++t)
    {
        this->operator[](t) += other[t];
    }
    return *this;
}

TimeDependentLinearExpression& TimeDependentLinearExpression::operator-=(
  const TimeDependentLinearExpression& other)
{
    if (other.size() > size())
    {
        expandTo(other.size());
    }
    for (std::size_t t = 0; t < size(); ++t)
    {
        this->operator[](t) -= other[t];
    }
    return *this;
}

void TimeDependentLinearExpression::rotate(int shift)
{
    if (shift == 0)
    {
        // Nothing to do
        return;
    }
    if (auto* expr = std::get_if<std::vector<LinearExpression>>(&v_); expr && !expr->empty())
    {
        const int n = static_cast<int>(expr->size());
        const int k = ((shift % n) + n) % n;
        std::rotate(expr->begin(), expr->begin() + k, expr->end());
    }
}

TimeDependentLinearExpression& TimeDependentLinearExpression::operator*=(double factor)
{
    for (auto& expr: *this)
    {
        expr *= factor;
    }
    return *this;
}

TimeDependentLinearExpression& TimeDependentLinearExpression::operator*=(
  const TimeDependentLinearExpression& other)
{
    if (other.size() > size())
    {
        expandTo(other.size());
    }
    int t = 0;
    for (std::size_t t = 0; t < size(); t++)
    {
        this->operator[](t) *= other[t];
    }
    return *this;
}

TimeDependentLinearExpression TimeDependentLinearExpression::operator-() const
{
    TimeDependentLinearExpression result = *this;
    for (auto& expr: result)
    {
        expr = -expr;
    }
    return result;
}
} // namespace Antares::Optimization
