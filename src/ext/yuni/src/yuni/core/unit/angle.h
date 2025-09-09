/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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
#define __YUNI_CORE_UNIT_ANGLE_H__

#include "unit.h"
#include "define.h"

namespace Yuni::Unit
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

} // namespace Angle
} // namespace Yuni::Unit

#include "undef.h"

#endif /// __YUNI_CORE_UNIT_ANGLE_H__
