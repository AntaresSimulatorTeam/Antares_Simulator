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
** SPDX-License-Identifier: MPL 2.0
*/

#include <antares/stdcxx/demangle.hpp>

namespace stdcxx
{
std::string demangle(const char* name)
{
    // TODO : for now no boost include, no demangle of class name
    /*
    #if defined(_WIN32) || defined(WIN32)
        // under windows typeid(T).name() does not return a mangled name, but returns :
        //  - "class T" if T is a class
        //  - "struct T" if T is a struct
        //  - "enum T" if T is an enum
        //  - "union T" if T is an union
        // so remove this useless prefix by removing everything found before the last ' ' character
        std::string simplifiedName = name;
        std::size_t index = simplifiedName.rfind(' ');
        if (index != std::string::npos) {
            simplifiedName = simplifiedName.substr(index + 1);
        }

        return boost::core::demangle(simplifiedName.c_str());
    #else
        return boost::core::demangle(name);
    #endif
    */

    return name;
}

template<>
std::string demangle(const std::type_info& type)
{
    return demangle(type.name());
}

std::string simpleClassName(const char* className)
{
    const std::string& strClassName = demangle(className);
    std::size_t index = strClassName.find_last_of("::");

    return (index == std::string::npos) ? strClassName
                                        : strClassName.substr(index + 1, strClassName.size());
}

template<>
std::string simpleClassName(const std::type_info& type)
{
    return simpleClassName(type.name());
}

} // namespace stdcxx