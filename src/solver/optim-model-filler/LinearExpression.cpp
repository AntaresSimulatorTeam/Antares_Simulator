/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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
    result.reserve(map.size());
    for (auto [key, value]: map)
    {
        result[key] = scale * value;
    }
    return result;
}

LinearExpression::LinearExpression(double offset, FullKeyMap coef_per_var):
    offset_(offset),
    terms_(),
    cache_(std::move(coef_per_var)),
    cacheValid_(true)
{
    terms_.reserve(cache_.size());
    for (const auto& [k, v]: cache_)
    {
        terms_.emplace_back(k, v);
    }
}

// Static helper: scale vector of terms
std::vector<LinearExpression::RawTerm> LinearExpression::scaleTerms(const std::vector<RawTerm>& src,
                                                                    double factor)
{
    constexpr double epsilon = 1e-12;
    if (std::abs(factor - 1.0) < epsilon)
    {
        return src; // copy elision
    }
    std::vector<RawTerm> out;
    out.reserve(src.size());
    for (const auto& [k, v]: src)
    {
        out.emplace_back(k, v * factor);
    }
    return out;
}

void LinearExpression::materialize() const
{
    if (cacheValid_)
    {
        return;
    }
    cache_.clear();
    cache_.reserve(terms_.size());
    for (const auto& [k, v]: terms_)
    {
        cache_[k] += v; // accumulate duplicates
    }
    cacheValid_ = true;
}

LinearExpression LinearExpression::operator+(const LinearExpression& other) const
{
    auto result(*this);
    result += other;
    return result;
}

const FullKeyMap& LinearExpression::coefPerVar() const
{
    materialize();
    return cache_;
}

LinearExpression& LinearExpression::operator+=(const LinearExpression& other)
{
    offset_ += other.offset_;
    if (!other.terms_.empty())
    {
        terms_.reserve(terms_.size() + other.terms_.size());
        terms_.insert(terms_.end(), other.terms_.begin(), other.terms_.end());
    }
    // If other had pre-materialized unique map but no raw terms (should not happen), ignore.
    invalidate();
    return *this;
}

LinearExpression LinearExpression::operator-(const LinearExpression& other) const
{
    auto result(*this);
    result += -other;
    return result;
}

LinearExpression LinearExpression::operator*(const LinearExpression& other) const
{
    if (terms_.empty())
    {
        // this is constant; scale other's terms
        auto scaledTerms = scaleTerms(other.terms_, offset_);
        LinearExpression out;
        out.offset_ = offset_ * other.offset_;
        out.terms_ = std::move(scaledTerms);
        out.invalidate();
        return out;
    }
    else if (other.terms_.empty())
    {
        auto scaledTerms = scaleTerms(terms_, other.offset_);
        LinearExpression out;
        out.offset_ = offset_ * other.offset_;
        out.terms_ = std::move(scaledTerms);
        out.invalidate();
        return out;
    }
    else
    {
        throw std::invalid_argument("A linear expression can't have quadratic terms.");
    }
}

LinearExpression LinearExpression::operator/(const LinearExpression& other) const
{
    if (!other.terms_.empty())
    {
        throw std::invalid_argument("A linear expression can't have a variable as a dividend.");
    }
    double inv = 1.0 / other.offset_;
    auto scaledTerms = scaleTerms(terms_, inv);
    LinearExpression out;
    out.offset_ = offset_ * inv;
    out.terms_ = std::move(scaledTerms);
    out.invalidate();
    return out;
}

LinearExpression LinearExpression::operator-() const
{
    auto scaledTerms = scaleTerms(terms_, -1.0);
    LinearExpression out;
    out.offset_ = -offset_;
    out.terms_ = std::move(scaledTerms);
    out.invalidate();
    return out;
}

double LinearExpression::offset() const
{
    return offset_;
}

} // namespace Antares::Optimization
