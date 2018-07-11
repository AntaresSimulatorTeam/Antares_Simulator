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
#ifndef __ANTARES_COMMON_COMPONENT_SPOTLIGHT_LISTBOX_PANEL_H__
# define __ANTARES_COMMON_COMPONENT_SPOTLIGHT_LISTBOX_PANEL_H__

#include <yuni/yuni.h>
#include "../../wx-wrapper.h"
#include <wx/scrolwin.h>
#include "spotlight.h"


namespace Antares
{
namespace Component
{


	class ListboxPanel : public wxScrolledWindow, public Yuni::IEventObserver<ListboxPanel>
	{
	public:
		enum
		{
			searchResultTextHeight = 25,
		};

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		ListboxPanel(Spotlight* parent, uint flags);
		//! Destructor
		virtual ~ListboxPanel();
		//@}

		void updateItems(const Spotlight::IItem::VectorPtr& vptr, const Spotlight::SearchToken::VectorPtr& tokens);

		/*!
		** \brief Reset the data provider
		**
		** It is likely that the provider has been changed from the component.
		*/
		void resetProvider(Spotlight::IProvider::Ptr provider);

	protected:
		//! Event: draw the panel
		void onDraw(wxPaintEvent& evt);
		//! Event: mouse moved
		void onMouseMotion(wxMouseEvent& evt);
		//! Event: mouse leave
		void onMouseLeave(wxMouseEvent&);
		//! UI: Erase background, empty to avoid flickering
		void onEraseBackground(wxEraseEvent&) {}
		//! Recompute the bounds of the longuest group name
		void recomputeBoundsForLonguestGroupName(wxDC& dc);

		//! Event: Mouse Click
		void onMouseLeftUp(wxMouseEvent& evt);
		//! Event: Mouse Double Click
		void onMouseDblClick(wxMouseEvent& evt);
		//! Delayed event
		void doSelectItemFromMouseLeftUp();
		//! Delayed event
		void doSelectItemFromMouseDblClick();

	private:
		//! The parent window
		Spotlight* pParent;
		//! Items
		Spotlight::IItem::VectorPtr pItems;
		//! Tokens
		Spotlight::SearchToken::VectorPtr pTokens;
		//! The longuest group name
		wxString pLonguestGroupName;
		wxSize pLonguestGroupNameSize;
		bool pRecomputeLonguestGroupNameSize;
		wxColour pDisabledColor;
		//!
		wxString pCacheResultText;
		wxSize pCacheResultTextSize;
		//
		uint pMouseOverItem;
		//! Provider
		Spotlight::IProvider::Ptr pProvider;
		//! Flags
		uint pFlags;

		//! Cache for last group name - used by onDraw
		YString pLastGroupName;

	protected:
		// Event table
		DECLARE_EVENT_TABLE()

	}; // class ListboxPanel





} // namespace Component
} // namespace Antares

#endif // __ANTARES_COMMON_COMPONENT_SPOTLIGHT_LISTBOX_PANEL_H__
