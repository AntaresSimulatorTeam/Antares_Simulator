#pragma once

#include <algorithm>
#include <ranges>
#include <vector>

namespace Antares::Solver::Simulation
{
inline bool operator<=(const std::vector<double>& a, const std::vector<double>& b)
{
    return a.size() == b.size()
           && std::ranges::all_of(std::views::iota(size_t{0}, a.size()),
                                  [&](size_t i) { return a[i] <= b[i]; });
}

inline bool operator<=(const std::vector<double>& v, const double c)
{
    return std::ranges::all_of(v, [&c](double e) { return e <= c; });
}

inline bool operator>=(const std::vector<double>& v, const double c)
{
    return std::ranges::all_of(v, [&c](double e) { return e >= c; });
}

inline std::vector<double> operator+(const std::vector<double>& v, const double c)
{
    std::vector<double> to_return = v;
    std::ranges::for_each(to_return, [&c](double& e) { e = e + c; });
    return to_return;
}

inline std::vector<double> operator-(const std::vector<double>& v, const double c)
{
    std::vector<double> to_return = v;
    std::ranges::for_each(to_return, [&c](double& e) { e = e - c; });
    return to_return;
}

}; // namespace Antares::Solver::Simulation
