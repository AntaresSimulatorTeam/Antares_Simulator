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
#pragma once
#include "math.h"

//! The constant value for `log(2)`
#define YUNI_LOG_2 0.301029996

namespace Yuni::Math
{
/*!
** \brief The natural logarithm function
*/
template<class T>
YUNI_DECL T Log(T x);

/*!
** \brief The base-2 logarithm function
*/
template<class T>
YUNI_DECL T Log2(T x);

/*!
** \brief Compute log(1 + x)
**
** If x is very small, directly computing log(1+x) can be inaccurate and the
** result may return log(1) = 0. All precision is lost.
** \code
** std::cout << Math::Log(1 + 1e-16) << std::endl; // = log(1) = 0
** \endcode
**
** We can avoid this problem by using a Taylor series
** log(1+x) ≈ x - (x*x)/2
**
** \param x Any value > -1.0
** \return log(1+x). when x < 1e-4, the Taylor series approximation will be used
*/
template<class T>
YUNI_DECL T LogOnePlusX(T x);

} // namespace Yuni::Math

#include "log.hxx"
