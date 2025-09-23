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
#pragma once
#include <stdexcept>
#include <utility>
#include <variant>
#include <vector>

inline void removeDuplicates(std::vector<std::pair<int, double>>& v)
{
    if (v.empty())
    {
        return;
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

namespace Antares::Optimization
{
struct LinearExpression final
{
    std::vector<std::pair<int, double>> coefs;
    double constant = 0.;

    LinearExpression() = default;

    LinearExpression(const std::vector<std::pair<int, double>>& coefs, double constant):
        coefs(coefs),
        constant(constant)
    {
    }

    void removeDuplicates()
    {
        ::removeDuplicates(coefs);
    }

    LinearExpression& operator+=(const LinearExpression& other)
    {
        coefs.reserve(coefs.size() + other.coefs.size());
        coefs.insert(coefs.end(), other.coefs.begin(), other.coefs.end());
        constant += other.constant;
        return *this;
    }

    LinearExpression& operator-=(const LinearExpression& other)
    {
        coefs.reserve(coefs.size() + other.coefs.size());
        for (const auto& [index, coef]: other.coefs)
        {
            coefs.emplace_back(index, -coef);
        }
        constant -= other.constant;
        return *this;
    }

    LinearExpression operator-() const
    {
        LinearExpression ret;
        ret.coefs.reserve(coefs.size());
        for (const auto& [index, coef]: coefs)
        {
            ret.coefs.emplace_back(index, -coef);
        }
        ret.constant = -constant;
        return ret;
    }

    LinearExpression& operator*=(const LinearExpression& other)
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
            constant *= other.constant;
        }
        else if (hasCoefs() && !other.hasCoefs())
        {
            // linear * constant
            for (auto& [idx, coef]: coefs)
            {
                coef *= other.constant;
            }
            constant *= other.constant;
        }
        else // (!hasCoefs() && other.hasCoefs())
        {
            // constant * linear
            coefs = other.coefs;
            for (auto& [idx, coef]: coefs)
            {
                coef *= constant; // use this->constant as multiplier
            }
            constant *= other.constant;
        }
        return *this;
    }

    bool hasCoefs() const
    {
        return !coefs.empty();
    }
};

class TimeDependentLinearExpression final
{
public:
    TimeDependentLinearExpression(std::size_t nbTimesteps)
    {
        if (nbTimesteps == 1)
        {
            v_.emplace<0>();
        }
        else
        {
            v_.emplace<1>(nbTimesteps);
        }
    }

    TimeDependentLinearExpression(LinearExpression&& expr):
        v_(std::move(expr))
    {
    }

    void expandTo(std::size_t nbTimesteps)
    {
        if (auto* expr = std::get_if<LinearExpression>(&v_))
        {
            v_.emplace<1>(nbTimesteps, *expr);
        }
    }

    std::vector<double> constant() const
    {
        if (auto* expr = std::get_if<LinearExpression>(&v_))
        {
            return {expr->constant};
        }
        if (auto* expr = std::get_if<std::vector<LinearExpression>>(&v_))
        {
            std::vector<double> ret;
            ret.reserve(expr->size());
            for (const auto& x: *expr)
            {
                ret.push_back(x.constant);
            }
            return ret;
        }
        throw std::runtime_error("Invalid variant");
    }

    void removeDuplicates()
    {
        for (auto& expr: *this)
        {
            expr.removeDuplicates();
        }
    }

    LinearExpression* begin()
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

    const LinearExpression* begin() const
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

    LinearExpression* end()
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

    const LinearExpression* end() const
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

    LinearExpression& operator[](std::size_t idx)
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

    const LinearExpression& operator[](std::size_t idx) const
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

    std::size_t size() const
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

    TimeDependentLinearExpression& operator+=(const TimeDependentLinearExpression& other)
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

    TimeDependentLinearExpression& operator-=(const TimeDependentLinearExpression& other)
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

    void rotate(int shift)
    {
        if (auto* expr = std::get_if<std::vector<LinearExpression>>(&v_); expr && !expr->empty())
        {
            const int n = static_cast<int>(expr->size());
            const int k = ((shift % n) + n) % n;
            std::rotate(expr->begin(), expr->begin() + k, expr->end());
        }
    }

    TimeDependentLinearExpression& operator*=(const TimeDependentLinearExpression& other)
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

    TimeDependentLinearExpression operator-() const
    {
        TimeDependentLinearExpression result = *this;
        for (auto& expr: result)
        {
            expr = -expr;
        }
        return result;
    }

private:
    std::variant<LinearExpression, std::vector<LinearExpression>> v_;
};
} // namespace Antares::Optimization
