// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef ANTARES_SOLVER_VARIABLE_COMMONS_HOUR_UTILS_H
#define ANTARES_SOLVER_VARIABLE_COMMONS_HOUR_UTILS_H

#include <concepts>
#include <cstddef>
#include <type_traits>

namespace Antares::Solver::Variable::Util
{

template<class T>
concept HourlySeries = requires(T t, unsigned y) {
    { t.series.getColumn(y) } -> std::convertible_to<const double*>;
    { t.series.timeSeries.height } -> std::convertible_to<size_t>;
};

template<class Ptr, class F>
inline void for_each_hour(const Ptr data, size_t n, F&& f) noexcept
{
    for (size_t h = 0; h < n; ++h)
    {
        f(h, data[h]);
    }
}

template<class Container, class F>
inline void for_each_hour(Container& c, F&& f) noexcept(noexcept(f(size_t{}, *c.data())))
{
    for (size_t h = 0; h < c.size(); ++h)
    {
        f(h, c[h]);
    }
}

template<class Range, class F>
inline void transform_in_place(Range&& r, F&& f) noexcept(noexcept(f(*r.begin())))
{
    for (auto& v: r)
    {
        v = f(v);
    }
}

} // namespace Antares::Solver::Variable::Util

#endif // ANTARES_SOLVER_VARIABLE_COMMONS_HOUR_UTILS_H
