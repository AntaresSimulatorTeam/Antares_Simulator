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
#ifndef __ANTARES_LIBS_RESOURCE_H__
# define __ANTARES_LIBS_RESOURCE_H__

# include <yuni/yuni.h>
# include <yuni/core/string.h>



namespace Antares
{
namespace Resources
{

	/*!
	** \brief Write the root folder into the log file
	*/
	void WriteRootFolderToLogs();


	/*!
	** \brief Get the full path of a resource file
	**
	** \param relFileName The filename of the resource, relative to the resource folder
	** \return The full path of the resource file if found, an empty string otherwise
	*/
	bool FindFile(YString& out, const AnyString& filename);


	/*!
	** \brief Get the full path of a resource file
	**
	** \param relFileName The filename of the resource, relative to the resource folder
	** \return The full path of the resource file if found, an empty string otherwise
	*/
	bool FindFirstOf(YString& out, const char* const * const list);


	/*!
	** \brief Try to find the examples folder
	*/
	bool FindExampleFolder(YString& folder);


	/*!
	** \brief Initialize variables about resource handling
	*/
	void Initialize(int argc, char* argv[], bool initializeSearchPath = false);


	/*!
	** \brief Copy the root folder
	*/
	void GetRootFolder(YString& out);





} // namespace Resources
} // namespace Antares

#endif // __ANTARES_LIBS_RESOURCE_H__
