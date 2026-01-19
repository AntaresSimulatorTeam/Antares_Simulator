// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef ANTARES_DATA_ENUM_HXX
#define ANTARES_DATA_ENUM_HXX

#include <algorithm>
#include <list>
#include <stdexcept>

#include <antares/enums/class_name.h>
#include "antares/exception/AssertionError.hpp"

namespace Antares::Data::Enum
{
template<typename E, typename>
E fromString(const std::string& name)
{
    const auto& names = getNames<E>();
    const auto& it = std::find(names.begin(), names.end(), name);
    if (it == names.end())
    {
        throw AssertionError("Unexpected " + stdcxx::simpleClassName<E>() + " name " + name);
    }

    return static_cast<E>(it - names.begin());
}

template<typename E, typename>
std::string toString(const E& value)
{
    auto index = static_cast<unsigned long>(value);
    const auto& names = getNames<E>();
    if (index >= names.size())
    {
        throw std::runtime_error("Unexpected " + stdcxx::simpleClassName<E>() + " value "
                                 + std::to_string(index));
    }
    return *(names.begin() + index);
}

template<typename E, typename>
std::list<E> enumList()
{
    std::list<E> result;
    const auto& names = getNames<E>();
    for (auto name: names)
    {
        result.push_back(fromString<E>(name));
    }

    return result;
}

} // namespace Antares::Data::Enum

#endif // ANTARES_DATA_ENUM_HXX
