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
