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
#ifndef __YUNI_CORE_UNIT_LENGTH_METRIC_H__
# define __YUNI_CORE_UNIT_LENGTH_METRIC_H__

# include "length.h"
# include "../unit.h"
# include "../define.h"



namespace Yuni
{
namespace Unit
{
namespace Length
{

	//! \brief Yottametre
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Yottametre, "yottameter", "Ym", int64_t, 1e24);
	//! \brief Zettametre
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Zettametre, "zettameter", "Zm", int64_t, 1e21);
	//! \brief Exametre
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Exametre, "exameter", "Em", int64_t, 1e18);
	//! \brief Petametre
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Petametre, "petameter", "Pm", int64_t, 1e15);
	//! \brief Terametre
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Terametre, "terameter", "Tm", int64_t, 1e12);
	//! \brief Gigametre
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Gigametre, "gigameter", "Gm", int, 1e9);
	//! \brief Megametre
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Megametre, "megameter", "Mm", int, 1e6);
	//! \brief Kilometre
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Kilometre, "kilometer", "km", int, 1e3);
	//! \brief Hectometre
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Hectometre, "hectometer", "hm", int, 1e2);
	//! \brief Decametre
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Decametre, "decameter", "dam", int, 10);


	//! \brief Metre (Distance light travels in 1/299 792 458 of a second in vacuum)
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Metre, "meter", "m", int,1);

	//! \brief Decimeter
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Decimetre, "decimetre", "dm", double, 1e-1);
	//! \brief Centimeter
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Centimetre, "centimetre", "cm", double, 1e-2);
	//! \brief Millimeter
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Millimetre, "millimetre", "mm", double, 1e-3);
	//! \brief Micrometre
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Micrometre, "micrometre", "µ,", double, 1e-6);
	//! \brief Nanometre
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Nanometre, "nanometre", "nm,", double, 1e-9);
	//! \brief Picometre
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Picometre, "picometre", "pm,", double, 1e-12);
	//! \brief Fentometre
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Fentometre, "fentometre", "fm,", double, 1e-15);
	//! \brief Attometre
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Attometre, "attometre", "am,", double, 1e-18);
	//! \brief Zeptometre
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Zeptometre, "zeptometre", "zm,", double, 1e-21);
	//! \brief Yoctometre
	//! \ingroup UnitLength
	YUNI_UNIT_IMPL(Yoctometre, "yoctometre", "ym,", double, 1e-24);



} // namespace Length
} // namespace Unit
} // namespace Yuni

# include "../undef.h"

#endif // __YUNI_CORE_UNIT_LENGTH_METRIC_H__
