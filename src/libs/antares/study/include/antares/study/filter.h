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
#ifndef __ANTARES_LIB_STUDY_FILTER_H__
#define __ANTARES_LIB_STUDY_FILTER_H__

#include <string>

#include <yuni/core/string.h>
#include <yuni/yuni.h>

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

// Convert a date precision filter into a string (obtained by concatenating filter components)
std::string datePrecisionIntoString(uint datePrecisionFilter);

/*!
** \brief Convert a string into date precision filter flags
*/
uint stringIntoDatePrecision(const AnyString& string);

/*!
** \brief Convert a filter indices into filter flags
*/
uint addTimeIntervallToDatePrecisionFilter(const uint index);

} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIB_STUDY_FILTER_H__
