// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optim-model-filler/LinearExpression.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

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
    coefs_.insert(coefs_.end(), other.coefs_.begin(), other.coefs_.end());
    constant_ += other.constant_;
    return *this;
}

LinearExpression& LinearExpression::operator-=(const LinearExpression& other)
{
    for (const auto& [index, coef]: other.coefs_)
    {
        coefs_.emplace_back(index, -coef);
    }
    constant_ -= other.constant_;
    return *this;
}

LinearExpression LinearExpression::operator/(const LinearExpression& other) const
{
    if (other.hasCoefs())
    {
        throw std::invalid_argument("A linear expression can't have a variable as a dividend.");
    }
    double inv = 1.0 / other.constant();
    LinearExpression out(*this);
    out *= inv;
    return out;
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

void LinearExpression::constant(double c)
{
    constant_ = c;
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
        throw std::invalid_argument("A linear expression can't have quadratic terms.");
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

static constexpr double EPS_TO_ZERO = 1e-16;

LinearExpression& LinearExpression::operator^=(const LinearExpression& other)
{
    if (other.hasCoefs())
    {
        throw std::invalid_argument("the exponent must be constant");
    }

    if (hasCoefs())
    {
        if (std::abs(other.constant()) < EPS_TO_ZERO)
        {
            bool isIdenticallyZero = (std::abs(constant_) < EPS_TO_ZERO)
                                     && std::ranges::all_of(coefs_,
                                                            [](const auto& coef) {
                                                                return std::abs(coef.second)
                                                                       < EPS_TO_ZERO;
                                                            });

            coefs_.clear();
            constant_ = isIdenticallyZero ? 0.0 : 1.0;
            return *this;
        }
        if (std::abs(other.constant() - 1) > EPS_TO_ZERO)
        {
            throw std::invalid_argument("non-linear expression is not supported.");
        }
    }
    constant_ = std::pow(constant_, other.constant_);
    return *this;
}

bool LinearExpression::hasCoefs() const
{
    return !coefs_.empty();
}

} // namespace Antares::Optimization
