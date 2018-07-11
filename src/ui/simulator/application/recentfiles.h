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
#ifndef __ANTARES_APPLICATION_RECENT_FILES_H__
# define __ANTARES_APPLICATION_RECENT_FILES_H__

# include <yuni/yuni.h>
# include "../toolbox/wx-wrapper.h"
# include <list>



namespace Antares
{
namespace RecentFiles
{
	//! The maximum number of recent files
	enum
	{
		Max = 10,
	};


	//! List of recent files
	typedef std::list< std::pair<wxString, wxString> >  List;
	//! Ptr
	typedef Yuni::SmartPtr<List> ListPtr;


	/*!
	** \brief Get the whole list of recent files
	*/
	ListPtr Get();


	/*!
	** \brief Insert a recent file
	*/
	void Add(const wxString& path, const wxString& title = wxString());

	/*!
	** \brief
	*/
	ListPtr AddAndGet(wxString path, const wxString& title = wxString());

	/*!
	** \brief Write the settings about the list of recent files
	*/
	void Write(const ListPtr lst);


	/*!
	** \brief Set if the path of the studies must be displayed in the menu
	*/
	void ShowPathInMenu(bool value);


	/*!
	** \brief Get if the path of the studies should be displayed in the menu
	*/
	bool ShowPathInMenu();




} // namespace RecentFiles
} // namespace Antares

#endif // __ANTARES_APPLICATION_RECENT_FILES_H__
