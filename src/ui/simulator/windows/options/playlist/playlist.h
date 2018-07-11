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
#ifndef __ANTARES_APPLICATION_WINDOWS_PLAYLIST_H__
# define __ANTARES_APPLICATION_WINDOWS_PLAYLIST_H__

# include <antares/wx-wrapper.h>
# include "../../../toolbox/components/datagrid/component.h"
# include <wx/dialog.h>
# include <wx/checkbox.h>


namespace Antares
{
namespace Window
{
namespace Options
{

	/*!
	** \brief Startup Wizard User Interface
	**
	** The Startup Wizard is an easy to use, quick way to begin using Antares.
	** It provides the most common File Operations on a Study.
	** By default, you will be prompted to use the Startup Wizard when Antares is opened.
	*/
	class MCPlaylist final : public wxDialog
	{
	public:
		enum IDs
		{
			mnIDEnabled = wxID_HIGHEST + 1,
		};

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		**
		** \param parent The parent window
		*/
		MCPlaylist(wxFrame* parent);
		//! Destructor
		virtual ~MCPlaylist();
		//@}


	protected:
		void mouseMoved(wxMouseEvent& evt);

	protected:
		//! Select all MC Years
		void onSelectAll(void*);
		void onUnselectAll(void*);
		void onToggle(void*);
		//! Event: Close the window
		void onClose(void*);
		void onUpdateStatus();
		void evtEnabled(wxCommandEvent& evt);
		void updateCaption();

	private:
		Component::Datagrid::Component* pGrid;
		wxCheckBox* pStatus;
		wxWindow* pPanel;
		wxWindow* pInfo;
		// Event table
		DECLARE_EVENT_TABLE()

	}; // class MCPlaylist






} // namespace Options
} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOWS_PLAYLIST_H__
