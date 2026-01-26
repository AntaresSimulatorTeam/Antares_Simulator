// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <cassert>

#include <boost/algorithm/string/case_conv.hpp>

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/io/file.h>

#include <antares/date/date.h>
#include <antares/logs/logs.h>

using namespace Yuni;

namespace Antares::Date
{
static const uint StandardDaysPerMonths[MONTHS_PER_YEAR] = {
  31, // january
  28, // february
  31, // march
  30, // april
  31, // may
  30, // june
  31, // july
  31, // august
  30, // september
  31, // october
  30, // november
  31, // december
};

static const char* const monthNames[] = {"January",
                                         "February",
                                         "March",
                                         "April",
                                         "May",
                                         "June",
                                         "July",
                                         "August",
                                         "September",
                                         "October",
                                         "November",
                                         "December"};

static const char* const monthNamesLower[] = {"january",
                                              "february",
                                              "march",
                                              "april",
                                              "may",
                                              "june",
                                              "july",
                                              "august",
                                              "september",
                                              "october",
                                              "november",
                                              "december"};

static const char* const monthShortNames[] = {"Jan",
                                              "Feb",
                                              "Mar",
                                              "Apr",
                                              "May",
                                              "Jun",
                                              "Jul",
                                              "Aug",
                                              "Sep",
                                              "Oct",
                                              "Nov",
                                              "Dec"};

static const char* const monthShortLowerNames[] = {"jan",
                                                   "feb",
                                                   "mar",
                                                   "apr",
                                                   "may",
                                                   "jun",
                                                   "jul",
                                                   "aug",
                                                   "sep",
                                                   "oct",
                                                   "nov",
                                                   "dec"};

static const char* const monthShortUpperNames[] = {"JAN",
                                                   "FEB",
                                                   "MAR",
                                                   "APR",
                                                   "MAY",
                                                   "JUN",
                                                   "JUL",
                                                   "AUG",
                                                   "SEP",
                                                   "OCT",
                                                   "NOV",
                                                   "DEC"};

uint DayInterval::numberOfWeeks() const
{
    return (end - first) / 7;
}

const char* DayOfTheWeekToString(DayOfTheWeek d)
{
    static const char* const days[] = {"Monday",
                                       "Tuesday",
                                       "Wednesday",
                                       "Thursday",
                                       "Friday",
                                       "Saturday",
                                       "Sunday"};

    return days[(uint)d % 7];
}

bool StringToMonth(MonthName& out, AnyString text)
{
    if (text.empty())
    {
        return false;
    }
    text.trim();
    std::string t = text;
    boost::algorithm::to_lower(t);
    /* t.toLower(); */

    if (t.size() == 3)
    {
        for (uint m = 0; m != MONTHS_PER_YEAR; ++m)
        {
            if (monthShortLowerNames[m] == t)
            {
                out = (MonthName)m;
                return true;
            }
        }
    }
    else
    {
        for (uint m = 0; m != MONTHS_PER_YEAR; ++m)
        {
            if (monthNamesLower[m] == t)
            {
                out = (MonthName)m;
                return true;
            }
        }
    }

    return false;
}

const char* MonthToString(int m, int offset)
{
    return monthNames[(m - offset) % MONTHS_PER_YEAR];
}

const char* MonthToLowerString(int m, int offset)
{
    return monthNamesLower[(m - offset) % MONTHS_PER_YEAR];
}

const char* MonthToShortString(int m, int offset)
{
    return monthShortNames[(m - offset) % MONTHS_PER_YEAR];
}

const char* MonthToUpperShortString(int m, int offset)
{
    return monthShortUpperNames[(m - offset) % MONTHS_PER_YEAR];
}

const char* WeekdayToString(int m)
{
    static const char* const wdays[] = {"Monday",
                                        "Tuesday",
                                        "Wednesday",
                                        "Thursday",
                                        "Friday",
                                        "Saturday",
                                        "Sunday"};
    return wdays[m % 7];
}

const char* WeekdayToShortString(int m)
{
    static const char* const wdays[] = {"Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"};
    return wdays[m % 7];
}

const char* WeekdayToShortUpperString(int m)
{
    static const char* const wdays[] = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
    return wdays[m % 7];
}

const char* WeekdayToShortLowerString(int m)
{
    static const char* const wdays[] = {"mon", "tue", "wed", "thu", "fri", "sat", "sun"};
    return wdays[m % 7];
}

const wchar_t* WeekdayToLShortString(int m)
{
    static const wchar_t* const wdays[] = {L"Mo", L"Tu", L"We", L"Th", L"Fr", L"Sa", L"Su"};
    return wdays[m % 7];
}

bool StringToDayOfTheWeek(DayOfTheWeek& v, const AnyString& text)
{
    if (text.empty())
    {
        v = monday;
        return false;
    }

    ShortString64 str = text;
    str.trim();
    if (str.empty())
    {
        v = monday;
        return false;
    }

    {
        uint t;
        if (str.to(t))
        {
            if (t < 7)
            {
                v = (DayOfTheWeek)t;
                return true;
            }
            v = monday;
            return false;
        }
    }

    str.toLower();

    switch (str.first())
    {
    case 'm':
    {
        if (str == "monday")
        {
            v = monday;
            return true;
        }
        if (str == "mardi")
        {
            return 0 != (v = tuesday);
        }
        if (str == "mercredi")
        {
            return 0 != (v = wednesday);
        }
        break;
    }
    case 't':
    {
        if (str == "tuesday")
        {
            return 0 != (v = tuesday);
        }
        if (str == "thursday")
        {
            return 0 != (v = thursday);
        }
        break;
    }
    case 'f':
    {
        if (str == "friday")
        {
            return 0 != (v = friday);
        }
        break;
    }

    case 'l':
    {
        if (str == "lundi")
        {
            v = monday;
            return true;
        }
        break;
    }
    case 's':
    {
        if (str == "saturday")
        {
            return 0 != (v = saturday);
        }
        if (str == "sunday")
        {
            return 0 != (v = sunday);
        }
        if (str == "samedi")
        {
            return 0 != (v = saturday);
        }
        break;
    }
    case 'w':
    {
        if (str == "wednesday")
        {
            return 0 != (v = wednesday);
        }
        break;
    }
    case 'j':
    {
        if (str == "jeudi")
        {
            return 0 != (v = thursday);
        }
        break;
    }
    case 'v':
    {
        if (str == "vendredi")
        {
            return 0 != (v = friday);
        }
        break;
    }
    case 'd':
    {
        if (str == "dimanche")
        {
            return 0 != (v = sunday);
        }
        break;
    }
    }
    // default
    v = monday;
    return false;
}

Calendar::Calendar()
{
    settings_.weekday1rstJanuary = monday;
    settings_.firstMonth = january;
    settings_.leapYear = false;
    settings_.weekFirstDay = monday;
    settings_.weekday1rstJanuary = monday;
}

void Calendar::reset(Settings settings)
{
    settings_ = settings;
    reset();
}

static inline DayOfTheWeek NextDayOfTheWeek(DayOfTheWeek weekday)
{
    return (weekday == sunday) ? monday : (DayOfTheWeek)(((uint)weekday) + 1);
}

void Calendar::reset()
{
#ifndef NDEBUG
    logs.debug() << "  reset calendar" << ", month : " << MonthToString(settings_.firstMonth)
                 << ", january 1rst : " << DayOfTheWeekToString(settings_.weekday1rstJanuary)
                 << ", first weekday : " << DayOfTheWeekToString(settings_.weekFirstDay);
#endif
    (void)::memset(hours, '\0', sizeof(hours));
    (void)::memset(days, '\0', sizeof(days));
    (void)::memset(weeks, '\0', sizeof(weeks));
    (void)::memset(months, '\0', sizeof(months));

    // Reset months relationship
    for (uint m = 0; m != MONTHS_PER_YEAR + 1; ++m)
    {
        uint realmonth = (m + (uint)settings_.firstMonth) % MONTHS_PER_YEAR;
        if (m < MONTHS_PER_YEAR)
        {
            mapping.months[realmonth] = m;
        }
        months[m].days = StandardDaysPerMonths[realmonth];
        months[m].realmonth = (MonthName)realmonth;

        auto& textmonth = text.months[m];
        textmonth.name = MonthToString(realmonth);
        textmonth.shortName = MonthToShortString(realmonth);
        textmonth.shortUpperName = MonthToUpperShortString(realmonth);
    }

    // leap year
    if (settings_.leapYear)
    {
        months[mapping.months[february]].days += 1;
    }

    // looking for the weekday of the first month
    auto weekday = settings_.weekday1rstJanuary;
    if (settings_.firstMonth != january)
    {
        // looking for the weekday of the first month of the simulation
        for (uint m = 11; m >= (uint)settings_.firstMonth; --m)
        {
            uint nbdays = (int)months[mapping.months[m]].days;
            for (uint d = 0; d != nbdays; ++d)
            {
                weekday = (weekday == monday) ? sunday : ((DayOfTheWeek)((uint)weekday - 1));
            }
        }
    }

    // initialization
    uint hourYear = 0;
    uint hourDay = 0;
    uint dayYear = 0;
    uint dayMonth = 0;
    uint currentMonth = 0;
    uint currentWeek = 0;

    // init days in the year
    days[0].hours.reset();
    days[0].weekday = weekday;
    days[0].month = 0;
    days[0].dayMonth = 0;
    days[0].week = 0;

    // init weeks
    weeks[0].hours.reset();
    weeks[0].daysYear.reset();
    // init months
    months[0].hours.reset();
    months[0].daysYear.reset();
    months[0].firstWeekday = weekday;

    for (; hourYear < maxHoursInYear; ++hourYear, ++hourDay)
    {
        // are we starting a new day ?
        if (hourDay >= 24) // 24 hour rotation
        {
            hourDay = 0;
            hours[hourYear].firstHourInDay = true;
            if (hourYear > 0) // the very first hour must be skipped
            {
                days[dayYear].hours.end = hourYear;

                // The day year should be incremented before assigning
                // The current day is not included
                ++dayYear;
                assert(dayYear < maxDaysInYear && "day of the year index overflow");
                days[dayYear].hours.first = hourYear;

                ++dayMonth;
                if (dayMonth >= months[currentMonth].days)
                {
                    months[currentMonth].daysYear.end = dayYear;
                    months[currentMonth].hours.end = hourYear;

                    dayMonth = 0;
                    hours[hourYear].firstHourInMonth = true;
                    ++currentMonth;
                    assert(currentMonth < 13 && "month index overflow");
                    months[currentMonth].daysYear.first = dayYear;
                    months[currentMonth].daysYear.end = dayYear;
                    months[currentMonth].hours.first = hourYear;
                    months[currentMonth].firstWeekday = NextDayOfTheWeek(weekday);
                }

                // day of the week rotation
                weekday = NextDayOfTheWeek(weekday);
                if (weekday == settings_.weekFirstDay)
                {
                    weeks[currentWeek].hours.end = hourYear;
                    weeks[currentWeek].daysYear.end = dayYear;
                    ++currentWeek; // next week
                    assert(currentWeek < maxWeeksInYear && "week index overflow");
                    weeks[currentWeek].hours.first = hourYear;
                    weeks[currentWeek].daysYear.first = dayYear;
                }

                auto& day = days[dayYear]; // current day year
                day.week = currentWeek;
                day.dayMonth = dayMonth;
                day.month = currentMonth;
                day.weekday = weekday;
            }
        }

        // updating calendar
        auto& ch = hours[hourYear]; // current hour
        ch.weekday = weekday;
        ch.dayYear = dayYear;
        ch.dayMonth = dayMonth;
        ch.month = currentMonth;
        ch.week = currentWeek;
        ch.dayHour = hourDay;
    }

    // final marks
    days[dayYear].hours.end = hourYear;
    weeks[currentWeek].hours.end = hourYear;
    weeks[currentWeek].daysYear.end = dayYear;
    months[currentMonth].daysYear.end = dayYear;
    months[currentMonth].hours.end = hourYear;

    // preparing user week numbers
    {
        // getting the day index of 1st january
        uint day1stIndex = months[mapping.months[january]].daysYear.first;
        // getting the week index of 1st january
        // int: to allow negative in this particular case
        int weekindex = (int)days[day1stIndex].week;
        assert(weekindex < maxWeeksInYear && "invalid week index");

        // always starting from 1
        uint startuserweek = 1;
        weeks[weekindex].userweek = startuserweek;

        // next weeks...
        uint userweek = (startuserweek == 53 ? 1 : 2);
        for (int w = weekindex + 1; w < maxWeeksInYear; ++w, ++userweek)
        {
            weeks[w].userweek = userweek;
        }

        // previous weeks...
        // userweek = (startuserweek == 1  ? 53 : 52);
        userweek = 52; // always 52 from now on
        for (int w = weekindex - 1; w >= 0; --w, --userweek)
        {
            weeks[w].userweek = userweek;
        }
    }

    // Caution :
    //		weeks[w].days are not computed here and are useless.
    //		their runtime counterparts are computed though (look in the study runtime infos).

    // preparing all text representations
    {
        for (uint hourYear = 0; hourYear != maxHoursInYear; ++hourYear)
        {
            auto& hourinfo = hours[hourYear];
            auto& hourtext = text.hours[hourYear];
            hourtext.clear();
            // Hour in the year - 1..8760
            uint hour = hourYear + 1;
            if (hour >= 1000)
            {
                hourtext << hour;
            }
            else if (hour < 10)
            {
                hourtext << "000" << hour;
            }
            else if (hour < 100)
            {
                hourtext << "00" << hour;
            }
            else if (hour < 1000)
            {
                hourtext << '0' << hour;
            }

            // week
            hourtext << " (";
            uint week = weeks[hourinfo.week].userweek;
            if (week >= 10)
            {
                hourtext << week;
            }
            else
            {
                hourtext << '0' << week;
            }
            hourtext << ')';

            hourtext << " - ";

            hourtext << WeekdayToShortUpperString((uint)hourinfo.weekday);
            hourtext << ' ';
            hourtext << text.months[hourinfo.month].shortUpperName;
            hourtext << ' ';
            if (hourinfo.dayMonth + 1 < 10)
            {
                hourtext << '0' << (hourinfo.dayMonth + 1);
            }
            else
            {
                hourtext << (hourinfo.dayMonth + 1);
            }

            hourtext << ' ';

            if (hourinfo.dayHour >= 10)
            {
                hourtext << hourinfo.dayHour;
            }
            else
            {
                hourtext << '0' << hourinfo.dayHour;
            }
            hourtext << ":00";
        }

        // days
        for (uint d = 0; d != maxDaysInYear; ++d)
        {
            auto& str = text.daysYear[d];
            str.clear();
            auto& dayinfo = days[d];
            uint day = d + 1;
            if (day < 10)
            {
                str << "00" << day;
            }
            else if (day < 100)
            {
                str << '0' << day;
            }
            else
            {
                str << day;
            }

            // week
            str << " (";
            uint week = weeks[dayinfo.week].userweek;
            if (week >= 10)
            {
                str << week;
            }
            else
            {
                str << '0' << week;
            }

            str << ") - ";
            str << WeekdayToShortUpperString((uint)dayinfo.weekday);
            str << ' ';
            str << text.months[dayinfo.month].shortUpperName;
            str << ' ';
            if (dayinfo.dayMonth + 1 < 10)
            {
                str << '0' << (dayinfo.dayMonth + 1);
            }
            else
            {
                str << (dayinfo.dayMonth + 1);
            }
        }
    }
}

} // namespace Antares::Date
