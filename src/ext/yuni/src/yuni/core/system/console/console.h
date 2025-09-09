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
#include "../../../yuni.h"

namespace Yuni::System
{
/*!
** \brief API for dealing with the attributes of characters written to the console screen buffer
*/
namespace Console
{
enum Color
{
    //! None
    none = 0,
    //! Black
    black,
    //! Red
    red,
    //! Green
    green,
    //! Yellow
    yellow,
    //! Blue
    blue,
    //! Purple (or Magenta on Windows)
    purple,
    //! Gray
    gray,
    //! White
    white,
    //! Lightblue (or cyan on Windows)
    lightblue,
    //! Bold
    bold,
};

/*!
** \brief Set the text color from a static constant
*/
template<int C>
struct YUNI_DECL TextColor final
{
    template<class U>
    static void Set(U& out);
};

/*!
** \brief Set the text color of the console
**
** \param[in,out] out An ostream (std::cout or std::cerr)
** \param color The new color
*/
template<class U>
YUNI_DECL void SetTextColor(U& out, const Color color);

/*!
** \brief Reset the text color to its default value
**
** \param[in,out] out An ostream (std::cout or std::cerr)
*/
template<class U>
YUNI_DECL void ResetTextColor(U& out);

/*!
** \brief Get if the standard output is a TTY
*/
YUNI_DECL bool IsStdoutTTY();

/*!
** \brief Get if the standard error output is a TTY
*/
YUNI_DECL bool IsStderrTTY();

} // namespace Console
} // namespace Yuni::System

#include "console.hxx"
