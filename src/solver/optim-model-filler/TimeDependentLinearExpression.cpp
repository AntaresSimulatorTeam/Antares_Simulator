// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optim-model-filler/TimeDependentLinearExpression.h"

#include <algorithm>
#include <stdexcept>

namespace Antares::Optimization
{

TimeDependentLinearExpression::TimeDependentLinearExpression(std::size_t nbTimesteps):
    v_(nbTimesteps)
{
}

TimeDependentLinearExpression::TimeDependentLinearExpression(const std::span<const double>& values)
{
    v_.reserve(values.size());
    for (const double& v: values)
    {
        v_.emplace_back(v);
    }
}

TimeDependentLinearExpression::TimeDependentLinearExpression(LinearExpression&& expr):
    v_(1, std::move(expr))
{
}

TimeDependentLinearExpression::TimeDependentLinearExpression(
  const std::vector<std::pair<int, double>>& coefs,
  double constant):
    v_(1, LinearExpression(coefs, constant))
{
}

void TimeDependentLinearExpression::expandTo(std::size_t nbTimesteps)
{
    v_.resize(nbTimesteps, v_[0]);
}

std::vector<double> TimeDependentLinearExpression::constant() const
{
    std::vector<double> ret;
    ret.reserve(this->size());
    for (const auto& x: *this)
    {
        ret.push_back(x.constant());
    }
    return ret;
}

void TimeDependentLinearExpression::mergeDuplicateCoefficients()
{
    for (auto& expr: *this)
    {
        expr.mergeDuplicateCoefficients();
    }
}

TimeDependentLinearExpression::iterator TimeDependentLinearExpression::begin()
{
    return v_.begin();
}

TimeDependentLinearExpression::const_iterator TimeDependentLinearExpression::begin() const
{
    return v_.begin();
}

TimeDependentLinearExpression::iterator TimeDependentLinearExpression::end()
{
    return v_.end();
}

TimeDependentLinearExpression::const_iterator TimeDependentLinearExpression::end() const
{
    return v_.end();
}

bool TimeDependentLinearExpression::isConstant() const
{
    return v_.size() == 1;
}

LinearExpression& TimeDependentLinearExpression::operator[](std::size_t idx)
{
    if (isConstant())
    {
        return v_[0];
    }
    else
    {
        return v_[idx];
    }
}

const LinearExpression& TimeDependentLinearExpression::operator[](std::size_t idx) const
{
    if (isConstant())
    {
        return v_[0];
    }
    else
    {
        return v_[idx];
    }
}

std::size_t TimeDependentLinearExpression::size() const
{
    return v_.size();
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
    if (shift == 0 || size() <= 1)
    {
        // Nothing to do
        return;
    }

    const int n = static_cast<int>(this->size());
    const int k = ((shift % n) + n) % n;
    std::rotate(this->begin(), this->begin() + k, this->end());
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
    for (std::size_t t = 0; t < size(); t++)
    {
        this->operator[](t) *= other[t];
    }
    return *this;
}

TimeDependentLinearExpression TimeDependentLinearExpression::operator/(
  const TimeDependentLinearExpression& other) const
{
    TimeDependentLinearExpression out(*this);
    if (other.size() > out.size())
    {
        out.expandTo(other.size());
    }
    for (std::size_t t = 0; t < size(); t++)
    {
        out[t] = out[t] / other[t];
    }
    return out;
}

TimeDependentLinearExpression TimeDependentLinearExpression::operator-() const
{
    TimeDependentLinearExpression result = *this;
    result *= -1.0;
    return result;
}
} // namespace Antares::Optimization
