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

#include "editor-arealinks.h"
#include "frame.h"
#include "../../application/menus.h"


using namespace Yuni;


namespace Antares
{
namespace Window
{
namespace Inspector
{




	wxPGWindowList AreaLinksBtnEditor::CreateControls(wxPropertyGrid* grid,
		wxPGProperty* property, const wxPoint& pos,	const wxSize& sz) const
	{
		// Create and populate buttons-subwindow
		wxPGMultiButton* buttons = new wxPGMultiButton(grid, sz);
		// Add two regular buttons
		buttons->Add(wxT(".."));
		// Create the 'primary' editor control (textctrl in this case)
		wxPGWindowList wndList = wxPGTextCtrlEditor::CreateControls(grid, property, pos, buttons->GetPrimarySize());
		buttons->Finalize(grid, pos);
		wndList.SetSecondary(buttons);
		return wndList;
	}



	bool AreaLinksBtnEditor::OnEvent(wxPropertyGrid* grid,
		wxPGProperty* property, wxWindow* ctrl, wxEvent& event) const
	{
		if (!gInspector)
			return false;
		// Since Connect() does not support userData (), we have to do it ourselves......
		gInspector->clearAssociatinsBetweenIDAndPtr();

		if (event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED)
		{
			wxPGMultiButton* buttons = (wxPGMultiButton*) grid->GetEditorControlSecondary();

			if (event.GetId() == buttons->GetButtonId(0))
			{
				delete pMenu;
				if (!gData || gData->areas.empty())
				{
					pMenu = nullptr;
					return false;
				}
				wxMenu* pMenu = new wxMenu();
				wxMenuItem* item;
				if (gData->areas.size() == 1)
					item = Menu::CreateItem(pMenu, wxID_ANY, wxT("Select all links from the area  "), "images/16x16/area.png");
				else
				{
					item = Menu::CreateItem(pMenu, wxID_ANY, wxString(wxT("Select all links from "))
						<< gData->areas.size() << wxT(" areas  "), "images/16x16/area.png");
				}
				pMenu->Connect(item->GetId(), wxEVT_COMMAND_MENU_SELECTED,
					wxCommandEventHandler(Frame::onSelectAllLinks), nullptr, gInspector);

				uint count = 0;
				wxString s;

				const Data::Area::Set::const_iterator areaEnd = gData->areas.end();
				for (Data::Area::Set::const_iterator i = gData->areas.begin(); i != areaEnd; ++i)
				{
					// The current area
					const Data::Area& area = *(*i);

					if (area.links.empty())
						continue;
					pMenu->AppendSeparator();

					if (gData->areas.size() > 1 && area.links.size() > 1)
					{
						item = Menu::CreateItem(pMenu, wxID_ANY, wxString(wxT("Select all links from "))
							<< wxStringFromUTF8(area.name), "images/16x16/area.png");
						gInspector->assign(item->GetId(), &area);
						pMenu->Connect(item->GetId(), wxEVT_COMMAND_MENU_SELECTED,
							wxCommandEventHandler(Frame::onSelectAllLinksFromArea), nullptr, gInspector);
					}

					const Data::AreaLink::Map::const_iterator end = area.links.end();
					Data::AreaLink::Map::const_iterator l = area.links.begin();
					for (; l != end; ++l)
					{
						s.clear();
						s << wxStringFromUTF8(area.name) << wxT("  /  ");
						s << wxStringFromUTF8(l->second->with->name);
						s << wxT("  "); // for beauty
						item = Menu::CreateItem(pMenu, wxID_ANY, s, "images/16x16/link.png");
						gInspector->assign(item->GetId(), l->second);
						pMenu->Connect(item->GetId(), wxEVT_COMMAND_MENU_SELECTED,
							wxCommandEventHandler(Frame::onSelectLink), nullptr, gInspector);
						++count;
					}
				}

				// fallback
				if (!count)
				{
					// No routine like `clear()` ???
					delete pMenu;
					pMenu = new wxMenu();
					wxMenuItem* it = Menu::CreateItem(pMenu, wxID_ANY, wxT("   No link available   "));
					it->Enable(false);
				}

				ctrl->PopupMenu(pMenu);

				// Do something when first button is pressed,
				// Return true if value in editor changed by this
				// action.
				return false;
			}
		}
		return wxPGTextCtrlEditor::OnEvent(grid, property, ctrl, event);
	}






} // namespace Inspector
} // namespace Window
} // namespace Antares

