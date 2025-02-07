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
#ifndef __ANTARES_LIBS_UTILS_H__
#define __ANTARES_LIBS_UTILS_H__

#include <string>
#include <vector>

#include <yuni/yuni.h>
#include <yuni/core/string.h>

namespace Antares
{
/*!
** \brief Transform an arbitrary string into an ID
**
** All invalid caracters will be replaced by `_`.
*/
template<class StringT>
void TransformNameIntoID(const AnyString& name, StringT& out);
std::string transformNameIntoID(const std::string& name);

std::tm getCurrentTime();
std::string formatTime(const std::tm& localTime, const std::string& format);

/*!
** \brief Beautify a name, for renaming an area for example
*/
void BeautifyName(YString& out, AnyString oldname);
void BeautifyName(std::string& out, const std::string& oldname);

std::vector<std::pair<std::string, std::string>> splitStringIntoPairs(const std::string& s,
                                                                      char delimiter1,
                                                                      char delimiter2);

namespace Utils
{

bool isZero(double d);
double round(double d, unsigned precision);
double ceilDiv(double numerator, double denominator);
double floorDiv(double numerator, double denominator);

} // namespace Utils
} // namespace Antares

#include "utils.hxx"

#endif // __ANTARES_LIBS_UTILS_H__
