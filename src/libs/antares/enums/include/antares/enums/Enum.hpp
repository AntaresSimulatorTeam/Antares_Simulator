// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef ANTARES_DATA_ENUM_HPP
#define ANTARES_DATA_ENUM_HPP

#include <initializer_list>
#include <list>
#include <string>
#include <type_traits>

namespace Antares::Data
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

} // namespace Antares::Data

#include <antares/enums/Enum.hxx>

#endif // ANTARES_DATA_ENUM_HPP
