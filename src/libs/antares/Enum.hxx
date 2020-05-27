/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#ifndef ANTARES_DATA_ENUM_HXX
#define ANTARES_DATA_ENUM_HXX

#include <algorithm>

#include <antares/exception/AssertionError.hpp>
#include <antares/stdcxx/demangle.hpp>

#include <antares/Enum.hpp>

namespace Antares
{
namespace Data
{
    
namespace Enum {

template <typename E, typename>
E fromString(const std::string& name) {
    const auto& names = getNames<E>();
    const auto& it = std::find(names.begin(), names.end(), name);
    if (it == names.end()) {
        throw AssertionError("Unexpected " + stdcxx::simpleClassName<E>() + " name " +name);
    }

    return static_cast<E>(it - names.begin());
}

template <typename E, typename>
bool isValid(const std::string& name) {
    bool result = true;
    const auto& names = getNames<E>();
    const auto& it = std::find(names.begin(), names.end(), name);
    if (it == names.end()) {
        result = false;
    }

    return result;
}

template <typename E, typename>
std::string toString(const E& value) {
    auto index = static_cast<unsigned long>(value);
    const auto& names = getNames<E>();
    if (index >= names.size()) {
        throw AssertionError("Unexpected " +  stdcxx::simpleClassName<E>() + " value " + std::to_string(index));
    }
    return *(names.begin() + index);
}

} // namespace Enum

} // namespace Data

} // namespace Antares

#endif  // ANTARES_DATA_ENUM_HXX