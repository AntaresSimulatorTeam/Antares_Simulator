#pragma once

#include <algorithm>
#include <ranges>
#include <span>
#include <stdexcept>
#include <vector>

namespace rng = std::ranges;

namespace Antares::Solver::Simulation
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
    rng::transform(a, b, a.begin(), std::plus<double>());
    return a;
}

inline std::vector<double> operator-(std::vector<double> a, const std::vector<double>& b)
{
    rng::transform(a, b, a.begin(), std::minus<double>());
    return a;
}

inline std::vector<double> operator*(std::span<const double> left, const double scalar)
{
    std::vector<double> to_return;
    rng::transform(left, std::back_inserter(to_return), [&](double e) { return e * scalar; });
    return to_return;
}

inline double min_on_subrange(std::vector<double>&& v, unsigned h, unsigned H)
{
    if (!v.size())
    {
        throw std::invalid_argument("call min_on_subrange on an empty vector");
    }

    if (H > v.size() || h > v.size())
    {
        throw std::invalid_argument("call of min_on_subrange : hour out of bound");
    }

    if (h > H)
    {
        throw std::invalid_argument("call min_on_subrange with inconsistant hours");
    }

    std::span<double> subset(v.begin() + h, v.begin() + H + 1);
    return *std::ranges::min_element(subset);
}

}; // namespace Antares::Solver::Simulation
