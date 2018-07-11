/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#ifndef __YUNI_CORE_UNIT_TIME_H__
# define __YUNI_CORE_UNIT_TIME_H__

# include "unit.h"
# include "define.h"

namespace Yuni
{
namespace Unit
{


/*!
** \defgroup UnitTime  Time
** \ingroup Units
*/

/*!
** \brief Time
** \ingroup UnitTime
*/
namespace Time
{
	//! \ingroup UnitTime
	struct Quantity;

	//! \brief SI (Second)
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(SIBaseUnit, "second", "s", int, 1);

	//! \brief Millenium (1000 years)
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Millenium, "millenium", "", int64_t, 31556952000);
	//! \brief Century (100 years)
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Century, "century", "c", int64_t, 3155695200);
	//! \brief Decade (10 years)
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Decade, "decade", "dec", int, 315569520);
	//! \brief Gregorian Year (365.2425 days)
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Year, "year", "a", int, 31556952);
	//! \brief Julian Year (365.25 days)
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(JulianYear, "julianyear", "", int, 31557600);
	//! \brief Sidereal Year (time taken for Sun to return to the same position with respect to the stars of the celestial sphere)
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(SiderealYear, "siderealyear", "", double, 31558149.7632);
	//! \brief Tropical Year (time it takes for the Sun to return to the same position in the cycle of seasons)
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(TropicalYear, "tropicalyear", "", int, 31556925);
	//! \brief Gregorian month (Average Gregorian calendar month duration)
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Month, "month", "mo", int, 2629700);
	//! \brief Synodic month (Average cycle time of moon phases)
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(SynodicMonth, "synodicmonth", "", int, 2551000);
	//! \brief Fortnight (2 weeks)
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Fortnight, "fortnight", "fn", int, 1209600);
	//! \brief Week (7 days)
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Week, "week", "wk", int, 604800);
	//! \brief Day (24 hours)
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Day, "day", "d", int, 86400);
	//! \brief Ke (Chinese calendar : 14.4 minutes since 100 ke make a day)
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Ke, "ke", "", 864);
	//! \brief Hour (60 minutes)
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Hour, "hour", "h", int, 3600);
	//! \brief Minute (60 seconds)
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Minute, "minute", "min", int, 60);
	//! \brief Helek (3 and a third seconds, since 1080 halakim make an hour in the Hebrew calendar)
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Helek, "helek", "", double, 10.0/3.0);

	//! \brief Second (Duration of 9,192,631,770 periods of the radiation corresponding to the transition between the two hyperfine levels of the ground state of the caesium-133 atom)
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Second, "second", "s", int, 1);

	//! \brief Decisecond
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Decisecond, "decisecond", "ds", double, 1e-1);
	//! \brief Centisecond
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Centisecond, "centisecond", "cs", double, 1e-2);
	//! \brief Millisecond
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Millisecond, "millisecond", "ms", double, 1e-3);
	//! \brief Microsecond
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Microsecond, "microsecond", "µs,", double, 1e-6);
	//! \brief Nanosecond
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Nanosecond, "nanosecond", "ns,", double, 1e-9);
	//! \brief Picosecond
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Picosecond, "picosecond", "ps,", double, 1e-12);
	//! \brief Fentosecond
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Fentosecond, "fentosecond", "fs,", double, 1e-15);
	//! \brief Attosecond
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Attosecond, "attosecond", "as,", double, 1e-18);
	//! \brief Zeptosecond
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Zeptosecond, "zeptosecond", "zs,", double, 1e-21);
	//! \brief Yoctosecond
	//! \ingroup UnitTime
	YUNI_UNIT_IMPL(Yoctosecond, "yoctosecond", "ys,", double, 1e-24);



} // namespace Time
} // namespace Unit
} // namespace Yuni

# include "../undef.h"

#endif // __YUNI_CORE_UNIT_TIME_H__
