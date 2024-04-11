/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#ifndef ANTARES_DATA_ENUM_HXX
#define ANTARES_DATA_ENUM_HXX

#include <algorithm>
#include <list>

#include <antares/exception/AssertionError.hpp>
#include <antares/stdcxx/class_name.h>
#include "antares/antares/Enum.hpp"

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
        throw AssertionError("Unexpected " + stdcxx::simpleClassName<E>() + " value "
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
