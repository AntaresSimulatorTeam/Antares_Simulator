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
#ifndef __ANTARES_LIBS_DATE_DATE_H__
# define __ANTARES_LIBS_DATE_DATE_H__

# include <yuni/yuni.h>
# include <yuni/core/string.h>
# include <vector>
# include "../constants.h"
# include "../study/fwd.h"


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





namespace Antares
{
namespace Date
{

	/*!
	** \brief Convert a month from its index to its string representation (January..December)
	*/
	const char* MonthToString(int m, int offset = 0);

	/*!
	** \brief Convert a month from its index to a short and upper case string representation (january..december)
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
	bool  StringToMonth(MonthName& out, AnyString text);

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
		//stepSecond,
		//stepMinute,
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
			end   = 0;
		}
	}; // class HourInterval

	struct DayInterval
	{
		//! The very first day
		uint first;
		//! The last day (not included)
		uint end;

		void reset()
		{
			first = 0;
			end   = 0;
		}
	}; // class DayInterval





	class Calendar
	{
	public:
		//! Short string representation with maximum 42 characters
		typedef Yuni::CString<41, false>  ShortString42;
		//! Short string representation with maximum 12 characters
		typedef Yuni::CString<11, false>  ShortString12;
		//! Short string representation with maximum 8 characters
		typedef Yuni::CString<7, false>  ShortString8;
		enum
		{
			//! The maximum number of hours in a year
			maxHoursInYear = 24 * 366,
			//! The maximum number of days in a year
			maxDaysInYear = 366,
			//! The maximum number of weeks in a year
			maxWeeksInYear = 54,
		};


	public:
		//!\name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		Calendar();
		//! Destructor
		~Calendar() {}
		//@}

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
		void reset(const Data::Parameters& parameters);

		/*!
		** \brief Reset the calendar from the given settings
		**
		** Reset the calendar from an instance of the class \p Parameters,
		** except for the leap year flag, which will be taken
		** from \p leapyear
		**
		** \param parameters Simulation settings
		** \param leapYear Force the value of the leap year
		*/
		void reset(const Data::Parameters& parameters, bool leapyear);

		/*!
		** \brief Export the whole calendar into a CSV file
		**
		** This method is mainly used for debug purposes
		*/
		bool saveToCSVFile(const AnyString& filename) const;


	public:
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
		}
		hours[maxHoursInYear];

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
		}
		days[maxDaysInYear];

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
		}
		weeks[maxWeeksInYear];

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
		}
		months[12 + 1]; // september..august for example

		/*!
		** \brief Mappings between any expected calendar and our own calendar
		*/
		struct
		{
			//! Mapping for months index
			uint months[12 /*january..december*/];
		}
		mapping;

		//! The calendar settings
		struct
		{
			//! Day of the week for the 1rst January
			DayOfTheWeek  weekday1rstJanuary;
			//! First day of weeks (for Antares simulations)
			DayOfTheWeek  weekFirstDay;
			//! Index (0..11) of the first month of the year
			MonthName firstMonth;
			//! Leap year ?
			bool leapYear;
		}
		settings;

		//! Human string representations for any time interval of our calendar
		struct
		{
			//! Caption for each month
			struct
			{
				//! Month name (January..December)
				ShortString12  name;
				//! Short name (3 letters) and uppercase (Jan..Dec)
				ShortString8   shortName;
				//! Short name (3 letters) and uppercase (JAN..DEC)
				ShortString8   shortUpperName;
			}
			months[12];

			//! Caption for each hour
			ShortString42  hours[maxHoursInYear];
			//! Caption for each day
			ShortString42  daysYear[maxHoursInYear];
			//! Weekdays
			ShortString12  weekdays[7];
		}
		text;

		struct
		{
			std::wstring  hours[maxHoursInYear];
		}
		wtext;

	}; // class Calendar










	// ------------------------------------------------------------
	//
	// DEPRECATED ROUTINES
	// -------------------
	//
	// Please consider the class calendar instead (accessible from
	// any antares study)
	// ------------------------------------------------------------



	/*!
	** \brief Informations about a single hour in the year
	*/
	class CalendarHour
	{
	public:
		//! Array
		typedef std::vector<CalendarHour> Vector;

	public:
		//! \name Constructor
		//@{
		/*!
		** \brief Default constructor
		*/
		CalendarHour();
		/*!
		** \brief Copy constructor
		*/
		CalendarHour(const CalendarHour& c);
		//@}

		/*!
		** \brief Add an hour in the calendar
		**
		** This method will update each entry in this class (except the string
		** representation).
		*/
		void addHour(const bool leapYear, int firstWeekDay, uint* firstDayPerWeek, uint* firstDayPerMonth);

		/*!
		** \brief Create the string representation
		**
		** \param indx An arbitrary index (zero-based)
		*/
		void toString(int indx);

		//! \name Operators
		//@{
		//! Copy
		CalendarHour& operator = (const CalendarHour& c);
		//@}

	public:
		//! The day in the year
		int dayYear;
		//! The hour in the day
		Yuni::sint8 hour;
		//! The week in the year
		Yuni::sint8 week;
		//! The day in the month
		Yuni::sint8 day;
		//! The month
		Yuni::sint8 month;
		//! The day in the week
		Yuni::sint8 weekday;
		//! A string representation
		Yuni::CString<41, false> str;
		//! First hour of the day
		bool firstHourOfTheDay;
		//! First hour of the month
		bool firstHourOfTheMonth;

	}; // class CalendarHour




	/*!
	** \brief Informations about a single day in the year
	*/
	class CalendarDay
	{
	public:
		//! Array
		typedef std::vector<CalendarDay> Vector;

	public:
		/*!
		** \brief Create the string representation
		**
		** \param indx An arbitrary index (zero-based)
		*/
		void toString(int indx);

	public:
		//! The month
		Yuni::sint8 month;
		//! Day in the month
		Yuni::sint8 day;
		//! The week in the year
		Yuni::sint8 week;
		//! Day in the week
		Yuni::sint8 weekday;
		//! A string representation
		Yuni::CString<41,false> str;

	}; // class CalendarDay









} // namespace Date
} // namespace Antares

#endif // __ANTARES_TOOLBOX_DATE_H__
