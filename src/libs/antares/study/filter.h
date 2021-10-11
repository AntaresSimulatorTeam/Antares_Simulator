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
#ifndef __ANTARES_LIB_STUDY_FILTER_H__
#define __ANTARES_LIB_STUDY_FILTER_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <string>

namespace Antares
{
namespace Data
{
enum FilterFlag
{
    filterNone = 0,
    filterHourly = 1,
    filterDaily = 2,
    filterWeekly = 4,
    filterMonthly = 8,
    filterAnnual = 16,
    filterAll = (uint)-1,
};

/*!
** \brief Append to an arbitrary string all flags that make up the filter
*/
template<class StringT>
void AppendFilterToString(StringT& out, uint filter);

std::string filterIntoString(uint filter);

/*!
** \brief Convert a mere string into filter flags
*/
uint StringToFilter(const AnyString& string);

/*!
** \brief Convert a filter indices into filter flags
*/
uint filterIndexToFilter(const uint index);

} // namespace Data
} // namespace Antares

#include "filter.hxx"

#endif // __ANTARES_LIB_STUDY_FILTER_H__
