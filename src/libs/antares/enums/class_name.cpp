// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/enums/class_name.h>

namespace stdcxx
{

std::string simpleClassName(const char* className)
{
    const std::string strClassName(className);
    std::size_t index = strClassName.rfind("::");

    return (index == std::string::npos)
             ? strClassName
             : strClassName.substr(index + 2); // +2 because "::" has length 2
}

template<>
std::string simpleClassName(const std::type_info& type)
{
    return simpleClassName(type.name());
}

} // namespace stdcxx
