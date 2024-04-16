/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#ifndef __ANTARES_LIBS_DATE_DATE_H__
#define __ANTARES_LIBS_DATE_DATE_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <vector>
#include "antares/study/fwd.h"

namespace Antares
{
//! Days of the week
enum DayOfTheWeek
{
    monday = 0,
    tuesday,
    wednesday,
    thursday,
    friday,
    saturday,
    sunday
};

//! Months
enum MonthName
{
    january = 0,
    february,
    march,
    april,
    may,
    june,
    july,
    august,
    september,
    october,
    november,
    december
};

} // namespace Antares

namespace Antares::Date
{
/*!
** \brief Convert a month from its index to its string representation (January..December)
*/
const char* MonthToString(int m, int offset = 0);

/*!
** \brief Convert a month from its index to a short and upper case string representation
*(january..december)
*/
const char* MonthToLowerString(int m, int offset = 0);

/*!
** \brief Convert a month from its index to a short string representation (Jan..Dec)
*/
const char* MonthToShortString(int m, int offset = 0);

/*!
** \brief Convert a month from its index to a short and upper case string representation (JAN..DEC)
*/
const char* MonthToUpperShortString(int m, int offset = 0);

/*!
** \brief Convert a text into a month
*/
bool StringToMonth(MonthName& out, AnyString text);

/*!
** \brief Convert a day from its index to its string representation (Monday..Sunday)
*/
const char* WeekdayToString(int m);

/*!
** \brief Convert a day from its index to its short string representation (Mo..Su)
*/
const char* WeekdayToShortString(int m);

/*!
** \brief Convert a day from its index to its short uppercase string representation (MON..SUN)
*/
const char* WeekdayToShortUpperString(int m);

/*!
** \brief Convert a day from its index to its short uppercase string representation (mon..sun)
*/
const char* WeekdayToShortLowerString(int m);

/*!
** \brief Convert a day from its index to its short string representation (Mo..Su)
*/
const wchar_t* WeekdayToLShortString(int m);

/*!
** \brief Try to convert a String into a day of the week
**
** \param[out] v The variable to modify
** \param text The string
** \return True if successful, 0 otherwise
*/
bool StringToDayOfTheWeek(DayOfTheWeek& v, const AnyString& text);

/*!
** \brief Convert a `day of the week` into its human readable value
**
** \param d A day of the week
** \return Its string representation
*/
const char* DayOfTheWeekToString(DayOfTheWeek d);

enum Precision
{
    //! No step
    stepNone = 0,
    // stepSecond,
    // stepMinute,
    hourly,
    daily,
    weekly,
    monthly,
    annual,
    //! All Steps
    stepAny,
};

struct HourInterval
{
    //! The very first hour
    uint first;
    //! The last hour (not included)
    uint end;

    void reset()
    {
        first = 0;
        end = 0;
    }
}; // class HourInterval

struct DayInterval
{
    //! The very first day
    uint first;
    //! The last day (not included)
    uint end;

    uint numberOfWeeks() const;

    void reset()
    {
        first = 0;
        end = 0;
    }
}; // class DayInterval

class Calendar
{
public:
    //! Short string representation with maximum 42 characters
    using ShortString42 = Yuni::CString<41, false>;
    //! Short string representation with maximum 12 characters
    using ShortString12 = Yuni::CString<11, false>;
    //! Short string representation with maximum 8 characters
    using ShortString8 = Yuni::CString<7, false>;
    enum
    {
        //! The maximum number of hours in a year
        maxHoursInYear = 24 * 366,
        //! The maximum number of days in a year
        maxDaysInYear = 366,
        //! The maximum number of weeks in a year
        maxWeeksInYear = 54,
    };

    //! informations about hours in the year according the current
    // calendar settings
    struct
    {
        //! Weekday
        DayOfTheWeek weekday;
        //! The day year for a single hour in the year
        uint dayYear;
        //! The day in the month
        uint dayMonth;
        //! Hour in the day
        uint dayHour;
        //! Month
        uint month;
        //! Week
        uint week;
        //! First hour in the current day
        bool firstHourInDay;
        //! First hour in the current month
        bool firstHourInMonth;
    } hours[maxHoursInYear];

    //! Informations about days in the year according the current
    // calendar settings
    struct
    {
        //! Weekday
        DayOfTheWeek weekday;
        //! Hour interval
        HourInterval hours;
        //! Month
        uint month;
        //! Day in the month
        uint dayMonth;
        //! Week
        uint week;
    } days[maxDaysInYear];

    /*!
    ** \brief Informations about weeks according the current calendar settings
    **
    ** We may have 53 weeks within a year (even if not complete)
    */
    struct
    {
        //! Hour interval
        HourInterval hours;
        //! Days in the year
        DayInterval daysYear;
        //! Total number of days of simulation for this week
        uint days;
        //! User week number
        // Final users prefer that weeks numbers are stable whatever the first month
        // of simulation is. If the 1st january is the same than the first weekday,
        // the week number for this day will be 1, 53 otherwise.
        uint userweek;
    } weeks[maxWeeksInYear];

    /*!
    ** \brief Informations about months according the current calendar settings
    **
    ** The index used here for months is not the standard one.
    ** The index 0 is settings.firstMonth (ex: september).
    ** The variable \p mapping.months must be used to get the
    ** real standard order.
    */
    struct
    {
        //! Hour interval
        HourInterval hours;
        //! Number of days
        uint days;
        //! Days in the year
        DayInterval daysYear;
        //! Very First weekday of the month
        DayOfTheWeek firstWeekday;
        //! Real month index
        MonthName realmonth;
    } months[12 + 1]; // september..august for example

    /*!
    ** \brief Mappings between any expected calendar and our own calendar
    */
    struct
    {
        //! Mapping for months index
        uint months[12 /*january..december*/];
    } mapping;

    //! The calendar settings
    struct Settings
    {
        //! Day of the week for the 1rst January
        DayOfTheWeek weekday1rstJanuary;
        //! First day of weeks (for Antares simulations)
        DayOfTheWeek weekFirstDay;
        //! Index (0..11) of the first month of the year
        MonthName firstMonth;
        //! Leap year ?
        bool leapYear;
    };

    //! Human string representations for any time interval of our calendar
    struct
    {
        //! Caption for each month
        struct
        {
            //! Month name (January..December)
            ShortString12 name;
            //! Short name (3 letters) and uppercase (Jan..Dec)
            ShortString8 shortName;
            //! Short name (3 letters) and uppercase (JAN..DEC)
            ShortString8 shortUpperName;
        } months[12];

        //! Caption for each hour
        ShortString42 hours[maxHoursInYear];
        //! Caption for each day
        ShortString42 daysYear[maxHoursInYear];
        //! Weekdays
        ShortString12 weekdays[7];
    } text;

    struct
    {
        std::wstring hours[maxHoursInYear];
    } wtext;

    //!\name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    Calendar();
    //! Destructor
    ~Calendar() = default;
    //@}

    Calendar(const Calendar&) = default;
    Calendar(Calendar&&) = default;
    Calendar& operator=(const Calendar&) = default;
    Calendar& operator=(Calendar&&) = default;

    /*!
    ** \brief Reset the calendar from the given settings
    */
    void reset();

    /*!
    ** \brief Reset the calendar from the given settings
    **
    ** Reset the calendar from an instance of the class \p Parameters,
    ** \param parameters Simulation settings
    */
    void reset(Settings settings);

    /*!
    ** \brief Export the whole calendar into a CSV file
    **
    ** This method is mainly used for debug purposes
    */
    bool saveToCSVFile(const AnyString& filename) const;

private:
    Settings settings_;

}; // class Calendar
} // namespace Antares::Date

#endif // __ANTARES_TOOLBOX_DATE_H__
