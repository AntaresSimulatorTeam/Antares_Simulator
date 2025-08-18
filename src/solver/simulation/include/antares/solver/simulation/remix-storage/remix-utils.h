#pragma once

#include <algorithm>
#include <ranges>
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

}; // namespace Antares::Solver::Simulation
