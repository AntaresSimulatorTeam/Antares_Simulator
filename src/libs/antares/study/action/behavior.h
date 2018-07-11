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
#ifndef __ANTARES_LIBS_STUDY_ACTION_BEHAVIOR_H__
# define __ANTARES_LIBS_STUDY_ACTION_BEHAVIOR_H__


namespace Antares
{
namespace Action
{

	/*!
	** \brief Behavior for a single action
	*/
	enum Behavior
	{
		//! Disabled the action and all sub-actions
		bhSkip = 0,
		/*!
		** \brief Create if not exists
		*/
		bhMerge,
		//! Overwrite any existing item
		bhOverwrite,

		//! The maximum number of behaviors
		bhMax

	}; // enum Behavior



	/*!
	** \brief Convert a behavior into its human string representation
	*/
	const char* BehaviorToString(Behavior behavior);


} // namespace Action
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_ACTION_BEHAVIOR_H__
