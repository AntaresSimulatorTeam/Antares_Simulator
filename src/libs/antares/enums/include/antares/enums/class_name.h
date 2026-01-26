// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>
#include <typeinfo>

namespace stdcxx
{

std::string simpleClassName(const char* className);

template<typename T>
std::string simpleClassName()
{
    return simpleClassName(typeid(T).name());
}

template<typename T>
std::string simpleClassName(const T& type)
{
    return simpleClassName(typeid(type).name());
}

} // namespace stdcxx
