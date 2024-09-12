/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
**
** SPDX-License-Identifier: MPL-2.0
*/
#ifndef ANTARES_DATA_ENUM_HPP
#define ANTARES_DATA_ENUM_HPP

#include <initializer_list>
#include <list>
#include <string>
#include <type_traits>

namespace Antares
{
namespace Data
{
namespace Enum
{
template<typename E, typename = typename std::enable_if<std::is_enum<E>::value>::type>
const std::initializer_list<std::string>& getNames();

template<typename E, typename = typename std::enable_if<std::is_enum<E>::value>::type>
std::string toString(const E& value);

template<typename E, typename = typename std::enable_if<std::is_enum<E>::value>::type>
E fromString(const std::string& name);

template<typename E, typename = typename std::enable_if<std::is_enum<E>::value>::type>
std::list<E> enumList();

} // namespace Enum

template<typename E>
inline typename std::enable_if<std::is_enum<E>::value, std::ostream&>::type operator<<(
  std::ostream& stream,
  const E& value)
{
    stream << Data::Enum::toString(value);
    return stream;
}

} // namespace Data

} // namespace Antares

#include <antares/antares/Enum.hxx>

#endif // ANTARES_DATA_ENUM_HPP
