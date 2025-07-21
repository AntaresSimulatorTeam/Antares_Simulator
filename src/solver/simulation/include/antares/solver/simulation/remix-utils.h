#pragma once

#include <algorithm>
#include <ranges>
#include <vector>

namespace Antares::Solver::Simulation
{
bool inline operator<=(const std::vector<double>& a, const std::vector<double>& b)
{
    return a.size() == b.size()
           && std::ranges::all_of(std::views::iota(size_t{0}, a.size()),
                                  [&](size_t i) { return a[i] <= b[i]; });
}

bool inline operator<=(const std::vector<double>& v, const double c)
{
    return std::ranges::all_of(v, [&c](double e) { return e <= c; });
}

bool inline operator>=(const std::vector<double>& v, const double c)
{
    return std::ranges::all_of(v, [&c](double e) { return e >= c; });
}
}; // namespace Antares::Solver::Simulation
