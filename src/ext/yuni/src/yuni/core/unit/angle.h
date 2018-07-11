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
#ifndef __YUNI_CORE_UNIT_ANGLE_H__
# define __YUNI_CORE_UNIT_ANGLE_H__

# include "unit.h"
# include "define.h"


namespace Yuni
{
namespace Unit
{

/*!
** \defgroup UnitAngle Angle
** \ingroup Units
*/

/*!
** \brief Angle
** \ingroup UnitAngle
*/
namespace Angle
{
	//! \ingroup UnitAngle
	struct Quantity;


	//! \brief SI (Radia)
	//! \ingroup UnitAngle
	YUNI_UNIT_IMPL(SIBaseUnit, "radian", "rad", int, 1);

	//! \brief Radian
	//! \ingroup UnitAngle
	YUNI_UNIT_IMPL(Radian, "radian", "rad", int, 1);

	//! \brief Degree (of arc)
	//! \ingroup UnitAngle
	YUNI_UNIT_IMPL(Degree, "degree", "°", double, 17.453293e-3);



} // namespace Length
} // namespace Unit
} // namespace Yuni

# include "undef.h"

#endif/// __YUNI_CORE_UNIT_ANGLE_H__
