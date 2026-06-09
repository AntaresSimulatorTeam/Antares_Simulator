// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <algorithm>
#include <numeric>
#include <ranges>
#include <span>
#include <vector>

namespace rng = std::ranges;

namespace Antares::Utils
{
inline bool operator<=(const std::vector<double>& a, const std::vector<double>& b)
{
    return a.size() == b.size()
           && rng::all_of(std::views::iota(size_t{0}, a.size()),
                          [&](size_t i) { return a[i] <= b[i]; });
}

inline bool operator<=(const std::vector<double>& v, const double c)
{
    return rng::all_of(v, [&c](double e) { return e <= c; });
}

inline bool operator>=(const std::vector<double>& v, const double c)
{
    return rng::all_of(v, [&c](double e) { return e >= c; });
}

inline std::vector<double> operator+(const std::vector<double>& v, const double c)
{
    std::vector<double> to_return = v;
    rng::for_each(to_return, [&c](double& e) { e = e + c; });
    return to_return;
}

inline std::vector<double> operator-(const std::vector<double>& v, const double c)
{
    std::vector<double> to_return = v;
    rng::for_each(to_return, [&c](double& e) { e = e - c; });
    return to_return;
}

inline std::vector<double> operator+(std::vector<double> a, const std::vector<double>& b)
{
    // Caution :
    // std::transform processes the min(a.size(), b.size()) first elements of a and b.
    // If a and b have diffrent size, no exception or undefined behavior.
    rng::transform(a, b, a.begin(), std::plus<double>());
    return a;
}

inline std::vector<double>& operator+=(std::vector<double>& a, const std::vector<double>& b)
{
    // Caution :
    // std::transform processes the min(a.size(), b.size()) first elements of a and b.
    // If a and b have diffrent size, no exception or undefined behavior.
    std::ranges::transform(a, b, a.begin(), std::plus<double>());
    return a;
}

inline std::vector<double> operator-(std::vector<double> a, const std::vector<double>& b)
{
    // Caution :
    // std::transform processes the min(a.size(), b.size()) first elements of a and b.
    // If a and b have diffrent size, no exception or undefined behavior.
    rng::transform(a, b, a.begin(), std::minus<double>());
    return a;
}

inline std::vector<double> operator*(std::span<const double> left, const double scalar)
{
    std::vector<double> to_return;
    rng::transform(left, std::back_inserter(to_return), [&](double e) { return e * scalar; });
    return to_return;
}

inline double sum(const std::vector<double>& v)
{
    return std::accumulate(v.begin(), v.end(), 0.);
}

}; // namespace Antares::Utils
