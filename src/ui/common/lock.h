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
#ifndef __ANTARES_UI_COMMON_LOCK_H__
# define __ANTARES_UI_COMMON_LOCK_H__

# include <yuni/yuni.h>


namespace Antares
{

	/*!
	** \brief The component should not perform their updates
	*/
	void GUIBeginUpdate();

	/*!
	** \brief The component may perform their graphical updates
	*/
	void GUIEndUpdate();

	/*!
	** \brief Get if the component can perform their updates
	*/
	bool GUIIsLock();


	//! Get the internal ref count value
	uint GUILockRefCount();


	/*!
	** \brief Lock / Unlock the GUI (RAII)
	*/
	class GUILocker final
	{
	public:
		GUILocker()
		{
			GUIBeginUpdate();
		}

		~GUILocker()
		{
			GUIEndUpdate();
		}
	};





	/*!
	** \brief Mark the GUI as 'about to quit', to avoid useless and sometimes costly refresh
	*/
	void GUIIsAboutToQuit();

	/*!
	** \brief The application is no longer quitting (ex: the user canceled the operation)
	*/
	void GUIIsNoLongerQuitting();

	/*!
	** \brief Get if the GUI is about to quit
	*/
	bool IsGUIAboutToQuit();



} // namespace Antares

#endif // __ANTARES_UI_COMMON_LOCK_H__
