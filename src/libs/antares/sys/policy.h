/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_LIBS_ADMIN_POLICY_H__
#define __ANTARES_LIBS_ADMIN_POLICY_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/core/singleton.h>

/*!
** \brief Local policy settings
*/
namespace LocalPolicy
{
//! A key entry
using PolicyKey = Yuni::ShortString64;

/*!
** \brief Open the local policies
**
** Nothing will be done (and will return true) if the local policies are already
** loaded.
** \param expandEntries True to expand the raw values
** \return True if some local policies have been found
*/
bool Open(bool expandEntries = true);

/*!
** \brief Close all resources related to the local policies
*/
void Close();

/*!
** \brief Check the root prefix, and abort immediatly if fails
*/
void CheckRootPrefix(const char* argv0);

/*!
** \brief Read an entry from the local policies
**
** If the local policies are not opened, it will always return false
** \return True if the entry exists, false otherwise
*/
bool HasKey(const PolicyKey& key);

/*!
** \brief Read the content of a single entry
**
** If the local policies are not opened, the value is guaranted to be empty.
** \return True if the entry exists, false otherwise
*/
bool Read(YString& out, const PolicyKey& key);

/*!
** \brief Read the content of a single entry as a bool
**
** If the local policies are not opened, the value is guaranted to be `defval`.
** \return True if the entry exists, false otherwise
*/
bool ReadAsBool(const PolicyKey& key, bool defval = false);

/*!
** \brief Dump the entries to the standard output
*/
void DumpToStdOut();

/*!
** \brief Dump the entries to the logs
*/
void DumpToLogs();

/*!
** \brief Dump to string
*/
void DumpToString(Yuni::Clob& out);

} // namespace LocalPolicy

#endif // __ANTARES_LIBS_ADMIN_POLICY_H__
