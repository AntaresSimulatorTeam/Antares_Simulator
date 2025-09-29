
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
#ifndef __YUNI_CORE_UNIT_LENGTH_LENGTH_H__
#define __YUNI_CORE_UNIT_LENGTH_LENGTH_H__

#include "../define.h"
#include "../unit.h"

namespace Yuni::Unit
{
/*!
** \defgroup UnitLength  Length
** \ingroup Units
*/

/*!
** \brief Length
** \ingroup Unit
*/
namespace Length
{
//! \ingroup UnitLength
struct Quantity;

//! \brief Metre (Distance light travels in 1/299 792 458 of a second in vacuum)
//! \ingroup UnitLength
YUNI_UNIT_IMPL(SIBaseUnit, "metre", "m", int, 1);

} // namespace Length
} // namespace Yuni::Unit

#include "../undef.h"

#endif /// __YUNI_CORE_UNIT_LENGTH_LENGTH_H__
