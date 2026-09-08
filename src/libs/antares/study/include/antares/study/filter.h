// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIB_STUDY_FILTER_H__
#define __ANTARES_LIB_STUDY_FILTER_H__

#include <string>

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
    filterAll = (unsigned int)-1,
};

// Convert a date precision filter into a string (obtained by concatenating filter components)
std::string datePrecisionIntoString(unsigned int datePrecisionFilter);

/*!
** \brief Convert a string into date precision filter flags
*/
unsigned int stringIntoDatePrecision(const std::string& string);

/*!
** \brief Convert a filter indices into filter flags
*/
unsigned int addTimeIntervallToDatePrecisionFilter(const unsigned int index);

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
