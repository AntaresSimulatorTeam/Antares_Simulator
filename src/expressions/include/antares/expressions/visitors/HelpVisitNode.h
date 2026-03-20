// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <algorithm>
#include <vector>

namespace Antares::Expressions::Visitors
{

template<typename T>
concept HasSizeMethod = requires(const T& t) {
    { t.size() } -> std::convertible_to<std::size_t>;
};

template<HasSizeMethod T>
std::size_t getMaxSize(const std::vector<T>& v)
{
    return std::ranges::max_element(v, {}, [](const T& e) { return e.size(); })->size();
}

} // namespace Antares::Expressions::Visitors
