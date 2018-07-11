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
#ifndef __ANTARES_TOOLBOX_INPUT_AREA_H__
# define __ANTARES_TOOLBOX_INPUT_AREA_H__

# include <antares/wx-wrapper.h>
# include <yuni/core/event.h>
# include <wx/panel.h>
# include <antares/study/study.h>
# include "input.h"
# include <wx/arrstr.h>
# include "../components/htmllistbox/component.h"


namespace Antares
{
namespace Toolbox
{
namespace InputSelector
{


	/*!
	** \brief Visual Component for displaying all available areas (and groups)
	*/
	class Area : public AInput, public Yuni::IEventObserver<Area>
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default Constructor
		** \param parent The parent window
		*/
		Area(wxWindow* parent);
		//! Destructor
		virtual ~Area();
		//@}

		virtual wxPoint recommendedSize() const {return wxPoint(230, 70);}

		/*!
		** \brief Get the last area selected by the component
		*/
		static Data::Area* lastArea();

		void reloadLastArea();

		virtual void update();

	public:
		/*!
		** \brief Public event, triggered when the selected area has changed
		*/
		static Yuni::Event<void (Data::Area*)> onAreaChanged;

	protected:
		virtual void internalBuildSubControls();

		/*!
		** \brief Clear the control and broadcast a change in the current selected area
		*/
		void clear();

		//! Callback: draw event
		void onDraw(wxPaintEvent& evt);

		//! The study has been updated
		void onStudyEndUpdate();

	private:
		void internalSelectionChanged();
		void onApplicationOnQuit();
		void onStudyClosed();

	private:
		//! Panel
		static wxPanel* pSharedSupport;
		//! The listbox, which will contains all items
		//static Component::HTMLListbox::Component* pAreaListbox;
		// Event table
		DECLARE_EVENT_TABLE()

	}; // class Area





} // namespace InputSelector
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_INPUT_AREA_H__
