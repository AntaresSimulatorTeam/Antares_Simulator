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
#ifndef __ANTARES_LIBS_STUDY_ACTION_FWD_H__
# define __ANTARES_LIBS_STUDY_ACTION_FWD_H__

# include <yuni/yuni.h>
# include <yuni/core/string.h>
# include "../study.h"
# include "behavior.h"
# include "state.h"


namespace Antares
{
namespace Action
{


	enum LevelOfDetails
	{
		//! For standard purposes
		lodStandard = 0,
		//! For advanced purposes
		lodAdvanced,
	};


	class IAction;

	//! Property map
	typedef std::map<Yuni::String, Yuni::String>  PropertyMap;




	class ActionInformations
	{
	public:
		ActionInformations()
			:behavior(bhOverwrite), state(stUnknown)
		{}

	public:
		//! Behavior of the action
		Behavior behavior;
		//! State of the action
		State state;
		//! Caption
		Yuni::String caption;
		//! Message
		Yuni::String message;
		//! Properties
		PropertyMap  property;

	}; // class ActionInformations



} // namespace Action
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_ACTION_FWD_H__
