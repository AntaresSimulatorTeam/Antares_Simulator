// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIB_STUDY_FILTER_H__
#define __ANTARES_LIB_STUDY_FILTER_H__

#include <string>

#include <yuni/yuni.h>
#include <yuni/core/string.h>

#include <antares/enums/Enum.hpp>

namespace Antares::Data
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

namespace Enum
{
template<>
inline const std::initializer_list<std::string>& getNames<FilterFlag>()
{
    static const std::initializer_list<std::string>
      il = {"none", "hourly", "daily", "weekly", "monthly", "annual", "all"};
    return il;
}
} // namespace Enum

} // namespace Antares::Data

#endif // __ANTARES_LIB_STUDY_FILTER_H__
