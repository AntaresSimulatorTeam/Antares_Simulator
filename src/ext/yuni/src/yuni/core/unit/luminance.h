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
#ifndef __YUNI_CORE_UNIT_LUMINANCE_H__
# define __YUNI_CORE_UNIT_LUMINANCE_H__

# include "unit.h"
# include "define.h"


namespace Yuni
{
namespace Unit
{


/*!
** \defgroup UnitLuminance  Luminance
** \ingroup Units
*/

/*!
** \brief Luminance
** \ingroup UnitLuminance
*/
namespace Luminance
{
	//! \ingroup UnitLuminance
	struct Quantity;

	//! \brief SI (Candela per square metre)
	//! \ingroup UnitLuminance
	YUNI_UNIT_IMPL(SIBaseUnit, "candela", "cd", int, 1);

	//! \brief Candela (per square metre)
	//! \ingroup UnitLuminance
	YUNI_UNIT_IMPL(Candela, "candela", "cd", int, 1);
	//! \brief Lambert
	//! \ingroup UnitLuminance
	YUNI_UNIT_IMPL(Lambert, "lambert", "L", double, 3183.09886);



} // namespace Luminance
} // namespace Unit
} // namespace Yuni

# include "undef.h"

#endif // __YUNI_CORE_UNIT_LUMINANCE_H__
