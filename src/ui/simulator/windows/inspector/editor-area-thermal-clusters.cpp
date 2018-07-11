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

#include "editor-area-thermal-clusters.h"
#include "frame.h"
#include "../../application/menus.h"


using namespace Yuni;


namespace Antares
{
namespace Window
{
namespace Inspector
{


	AreaThermalClustersBtnEditor::AreaThermalClustersBtnEditor() :
		pMenu(nullptr)
	{}


	AreaThermalClustersBtnEditor::~AreaThermalClustersBtnEditor()
	{
		delete pMenu;
	}


	wxPGWindowList AreaThermalClustersBtnEditor::CreateControls(wxPropertyGrid* grid,
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



	bool AreaThermalClustersBtnEditor::OnEvent(wxPropertyGrid* grid,
		wxPGProperty* property, wxWindow* ctrl, wxEvent& event) const
	{
		if (!gInspector)
			return false;

		if (event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED)
		{
			// Since Connect() does not support userData (), we have to do it ourselves......
			gInspector->clearAssociatinsBetweenIDAndPtr();

			auto* buttons = (wxPGMultiButton*) grid->GetEditorControlSecondary();

			if (event.GetId() == buttons->GetButtonId(0))
			{
				if (pMenu)
					delete pMenu;
				// inspector data
				auto inspectorData = gData;
				if (!inspectorData || inspectorData->areas.empty())
				{
					pMenu = nullptr;
					return false;
				}
				wxMenu* pMenu = new wxMenu();
				wxMenuItem* item;
				if (inspectorData->areas.size() == 1)
					item = Menu::CreateItem(pMenu, wxID_ANY, wxT("Select all thermal clusters from the area  "), "images/16x16/area.png");
				else
				{
					item = Menu::CreateItem(pMenu, wxID_ANY, wxString(wxT("Select all thermal clusters from "))
						<< inspectorData->areas.size() << wxT(" areas  "), "images/16x16/area.png");
				}
				pMenu->Connect(item->GetId(), wxEVT_COMMAND_MENU_SELECTED,
					wxCommandEventHandler(Frame::onSelectAllPlants), nullptr, gInspector);

				// Installed
				wxMenuItem* itemInstalled = Menu::CreateItem(pMenu, wxID_ANY, wxT("0 unit, 0 MW"));
				uint unitCount = 0;
				double installed = 0.;
				itemInstalled->Enable(false);

				uint count = 0;
				wxString s;

				auto areaEnd = inspectorData->areas.cend();
				for (auto i = inspectorData->areas.cbegin(); i != areaEnd; ++i)
				{
					// The current area
					const Data::Area& area = *(*i);

					if (area.thermal.list.empty())
						continue;
					pMenu->AppendSeparator();

					if (inspectorData->areas.size() > 1 && area.thermal.list.size() > 1)
					{
						item = Menu::CreateItem(pMenu, wxID_ANY, wxString(wxT("Select all thermal clusters from "))
								<< wxStringFromUTF8(area.name), "images/16x16/area.png");
						gInspector->assign(item->GetId(), &area);
						pMenu->Connect(item->GetId(), wxEVT_COMMAND_MENU_SELECTED,
							wxCommandEventHandler(Frame::onSelectAllPlantsFromArea), nullptr, gInspector);
					}

					auto end = area.thermal.list.end();
					for (auto l = area.thermal.list.begin(); l != end; ++l)
					{
						Data::ThermalCluster& cluster = *(l->second);
						unitCount += cluster.unitCount;
						installed += cluster.unitCount * cluster.nominalCapacity;

						s.clear();
						s << wxStringFromUTF8(area.name) << wxT("  ::  ");
						s << wxStringFromUTF8(l->second->name());
						s << wxT("  "); // for beauty
						item = Menu::CreateItem(pMenu, wxID_ANY, s, "images/16x16/thermal.png");
						gInspector->assign(item->GetId(), l->second);
						pMenu->Connect(item->GetId(), wxEVT_COMMAND_MENU_SELECTED,
							wxCommandEventHandler(Frame::onSelectPlant), nullptr, gInspector);
						++count;
					}
				}

				// fallback
				if (!count)
				{
					// No routine like `clear()` ???
					delete pMenu;
					pMenu = new wxMenu();
					wxMenuItem* it = Menu::CreateItem(pMenu, wxID_ANY, wxT("   No thermal cluster available   "));
					it->Enable(false);
				}
				else
				{
					if (unitCount)
					{
						s.clear();
						s << unitCount << ((unitCount > 1) ? wxT(" units, ") : wxT(" unit, "));
						s << DoubleToWxString(installed) << wxT(" MW");
						itemInstalled->SetItemLabel(s);
					}
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

