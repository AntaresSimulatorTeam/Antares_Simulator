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
#include "../../yuni.h"
#include "../../core/string.h"
#include "../../core/bind.h"

namespace Yuni::IO
{
/*
** \brief Directory manipulation functions
*/
namespace Directory
{
//! \name Exists
//@{
/*!
** \brief Test if a node exists and is actually a directory
**
** To test if a node exists whatever its nature, you should use
** IO::Exists() instead.
**
** \param path The directory to test
** \return True if it exists, false otherwise
*/
bool Exists(const AnyString& path);

//@}

/*!
** \brief Routines about current directories
*/
namespace Current
{
/*!
** \brief Get the current directory
**
** \param[out] out The variable where the current directory will be written
** \param clearBefore True to clean \p out before
** \return True if the operation succeeded. False otherwise
*/
bool Get(String& out, bool clearBefore = true);

/*!
** \brief Get the current directory (clob)
**
** \param[out] out The variable where the current directory will be written
** \param clearBefore True to clean \p out before
** \return True if the operation succeeded. False otherwise
*/
bool Get(Clob& out, bool clearBefore = true);

/*!
** \brief Get the current directory
**
** Performance Tip: Prefer Get(String&) which avoids several string copies
** \return The current directory
*/
String Get();

/*!
** \brief Set the current directory
**
** \param path The new current directory
** \return True if the operation succeeded
*/
bool Set(const AnyString& path);

} // namespace Current

//! \name Create a directory
//@{
/*!
** \brief Create a directory recursively
**
** \param path The path to create if it does not exist
** \param mode Access permissions (ignored on the MS Windows platforms)
** \return True if the operation succeeded, false otherwise
*/
bool Create(const AnyString& path, unsigned int mode = 0755);
//@}

//! \name Remove a directory
//@{
/*!
** \brief Recursively delete a directory and its content
**
** \param path The path to delete
** \return True if the operation succeeded False otherwise
*/
bool Remove(const AnyString& path);

//@}

//! \name Copy a directory
//@{
enum CopyState
{
    cpsGatheringInformation,
    cpsCopying
};

typedef Yuni::Bind<bool(CopyState, const String&, const String&, uint64_t, uint64_t)>
  CopyOnUpdateBind;

/*!
** \brief Copy a directory
**
** \param source The source folder
** \param destination The destination folder
** \param recursive True to copy recursively
** \param overwrite True to overwrite the files even if they already exist
** \return True if the operation succeeded, false otherwise
*/
bool Copy(const AnyString& source,
          const AnyString& destination,
          bool recursive = true,
          bool overwrite = true);

/*!
** \brief Copy a directory
**
** \param source The source folder
** \param destination The destination folder
** \param onUpdate Event
** \return True if the operation succeeded, false otherwise
*/
bool Copy(const AnyString& source, const AnyString& destination, const CopyOnUpdateBind& onUpdate);

/*!
** \brief Copy a directory
**
** \param source The source folder
** \param destination The destination folder
** \param recursive True to copy recursively
** \param overwrite True to overwrite the files even if they already exist
** \param onUpdate Event
** \return True if the operation succeeded, false otherwise
*/
bool Copy(const AnyString& source,
          const AnyString& destination,
          bool recursive,
          bool overwrite,
          const CopyOnUpdateBind& onUpdate);
//@}

} // namespace Directory
} // namespace Yuni::IO

#include "../io.h"
#include "directory.hxx"
