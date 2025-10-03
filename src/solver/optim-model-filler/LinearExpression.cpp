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

#include "antares/solver/optim-model-filler/LinearExpression.h"

#include <algorithm>
#include <stdexcept>

namespace
{
void removeDuplicates(std::vector<std::pair<int, double>>& v)
{
    // Most constraints have 1 or 2 terms, so we handle them here without sort & loops
    switch (v.size())
    {
    case 0:
    case 1:
        return;
    case 2:
        if (v[0].first != v[1].first)
        {
            return;
        }
        break;
    }
    // Step 1: sort by first
    std::sort(v.begin(), v.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

    // Step 2: merge duplicates
    size_t write = 0;
    for (size_t read = 1; read < v.size(); ++read)
    {
        if (v[read].first == v[write].first)
        {
            v[write].second += v[read].second; // accumulate
        }
        else
        {
            ++write;
            v[write] = v[read]; // move next unique element forward
        }
    }

    // Step 3: erase leftover duplicates
    v.erase(v.begin() + write + 1, v.end());
}
} // namespace

namespace Antares::Optimization
{
LinearExpression::LinearExpression() = default;

LinearExpression::LinearExpression(double constant):
    constant_(constant)
{
}

LinearExpression::LinearExpression(const std::vector<std::pair<int, double>>& coefs,
                                   double constant):
    coefs_(coefs),
    constant_(constant)
{
}

const std::pair<int, double>& LinearExpression::operator[](std::size_t n) const
{
    return coefs_[n];
}

std::size_t LinearExpression::size() const
{
    return coefs_.size();
}

void LinearExpression::removeDuplicateCoefficients()
{
    removeDuplicates(coefs_);
}

LinearExpression& LinearExpression::operator*=(double factor)
{
    for (auto& [index, coef]: coefs_)
    {
        coef *= factor;
    }
    constant_ *= factor;

    return *this;
}

LinearExpression& LinearExpression::operator+=(const LinearExpression& other)
{
    coefs_.reserve(coefs_.size() + other.coefs_.size());
    coefs_.insert(coefs_.end(), other.coefs_.begin(), other.coefs_.end());
    constant_ += other.constant_;
    return *this;
}

LinearExpression& LinearExpression::operator-=(const LinearExpression& other)
{
    coefs_.reserve(coefs_.size() + other.coefs_.size());
    for (const auto& [index, coef]: other.coefs_)
    {
        coefs_.emplace_back(index, -coef);
    }
    constant_ -= other.constant_;
    return *this;
}

LinearExpression LinearExpression::operator-() const
{
    LinearExpression ret;
    ret.coefs_.reserve(coefs_.size());
    for (const auto& [index, coef]: coefs_)
    {
        ret.coefs_.emplace_back(index, -coef);
    }
    ret.constant_ = -constant_;
    return ret;
}

double LinearExpression::constant() const
{
    return constant_;
}

LinearExpression::const_iterator LinearExpression::begin() const
{
    return coefs_.begin();
}

LinearExpression::const_iterator LinearExpression::end() const
{
    return coefs_.end();
}

void LinearExpression::addVariable(int index, double value)
{
    coefs_.emplace_back(index, value);
}

LinearExpression& LinearExpression::operator*=(const LinearExpression& other)
{
    if (hasCoefs() && other.hasCoefs())
    {
        // Multiplying two symbolic expressions would give quadratic terms,
        // which this representation cannot hold.
        throw std::runtime_error("Quadratic term detected");
    }
    else if (!hasCoefs() && !other.hasCoefs())
    {
        // constant * constant
        constant_ *= other.constant_;
    }
    else if (hasCoefs() && !other.hasCoefs())
    {
        // linear * constant
        for (auto& [idx, coef]: coefs_)
        {
            coef *= other.constant_;
        }
        constant_ *= other.constant_;
    }
    else // (!hasCoefs() && other.hasCoefs())
    {
        // constant * linear
        coefs_ = other.coefs_;
        for (auto& [idx, coef]: coefs_)
        {
            coef *= constant_; // use this->constant as multiplier
        }
        constant_ *= other.constant_;
    }
    return *this;
}

bool LinearExpression::hasCoefs() const
{
    return !coefs_.empty();
}

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

void TimeDependentLinearExpression::removeDuplicateCoefficients()
{
    for (auto& expr: *this)
    {
        expr.removeDuplicateCoefficients();
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
    for (auto& expr: *this)
    {
        expr *= other[t];
        t++;
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
