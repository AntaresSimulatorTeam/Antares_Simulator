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

#include "antares/solver/optim-model-filler/TimeDependentLinearExpression.h"

namespace
{
void mergeDuplicates(std::vector<std::pair<int, double>>& v)
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

void LinearExpression::mergeDuplicateCoefficients()
{
    mergeDuplicates(coefs_);
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

    else if (hasCoefs() && !other.hasCoefs())
    {
        // linear * constant
        for (auto& [idx, coef]: coefs_)
        {
            coef *= other.constant_;
        }
    }
    else if (!hasCoefs() && other.hasCoefs())
    {
        // constant * linear
        coefs_ = other.coefs_;
        for (auto& [idx, coef]: coefs_)
        {
            coef *= constant_; // use this->constant as multiplier
        }
    }
    // Also if (!hasCoefs() && !other.hasCoefs())
    constant_ *= other.constant_;
    return *this;
}

bool LinearExpression::hasCoefs() const
{
    return !coefs_.empty();
}

} // namespace Antares::Optimization
