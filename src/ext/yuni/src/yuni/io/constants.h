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
#include "io.h"

namespace Yuni::IO
{
/*!
** \brief Flow control used in the IO module
*/
enum Flow
{
    //! Abort the whole process
    flowAbort = 0,
    //! Continue
    flowContinue,
    //! Skip the current item
    flowSkip,
};

/*!
** \brief Type of a single node (bitmask)
*/
enum NodeType
{
    //! The node doest not exist
    typeUnknown = 0,
    //! The node is a folder
    typeFolder = 1,
    //! The node is a file
    typeFile = 2,
    //! The node is a symlink
    typeSymlink = 3,
    //! The node is a socket
    typeSocket = 4,
    //! The node is special (pipe, block special...)
    typeSpecial = 5,
};

//! \name System-dependant variables
//@{
/*!
** \brief The path-separator character according to the platform (ex: `/`)
*/
extern const char Separator; // '/'

/*!
** \brief The path-separator character according to the platform (stored in a string instead of a
*char)
*/
extern const char* SeparatorAsString; // "/"

/*!
** \brief Constant acoording a type
**
** These variables are identical to SeparatorAsString and Separator
** but are charactere-type dependant.
*/
template<typename C /* = char*/>
struct Constant;

template<>
struct Constant<char>
{
    // The complete specialization with wchar_t is in directory.hxx

    //! The path-separator character according to the platform (ex: `/`)
    static const char Separator; // '/';
    //! The path-separator character according to the platform (stored in a string instead of a
    //! char)
    static const char* SeparatorAsString; // "/"
    //! All path-separator characters, for all platforms
    static const char* AllSeparators; // "\\/"

    //! Dot
    static const char Dot; // '.';

}; // class Constant<char>

template<>
struct Constant<wchar_t>
{
    //! The path-separator character according to the platform (ex: `/`)
    static const wchar_t Separator; // L'/';
    //! The path-separator character according to the platform (stored in a string instead of a
    //! char)
    static const wchar_t* SeparatorAsString; // = L"/";
    //! All path-separator characters, for all platforms
    static const wchar_t* AllSeparators; // = L"\\/";
    //! Dot
    static const wchar_t Dot; // L'.';
};

//@}

} // namespace Yuni::IO
