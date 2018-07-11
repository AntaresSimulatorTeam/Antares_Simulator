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

#include "prepro.h"
#include <wx/stattext.h>
#include "../../toolbox/components/datagrid/renderer/area/hydroprepro.h"
#include "../../toolbox/components/datagrid/renderer/area/hydromonthlypower.h"
#include "../../toolbox/components/button.h"
#include "../../toolbox/validator.h"
#include "../../toolbox/create.h"
#include "../../application/menus.h"
#include <wx/statline.h>


using namespace Yuni;


namespace Antares
{
namespace Window
{
namespace Hydro
{


	Prepro::Prepro(wxWindow* parent, Toolbox::InputSelector::Area* notifier) :
		Component::Panel(parent),
		pInputAreaSelector(notifier),
		pArea(nullptr),
		pComponentsAreReady(false),
		pSupport(nullptr),
		pIntermonthlyBreakdown(nullptr),
		pIntermonthlyCorrelation(nullptr),
		pInterdailyBreakdown(nullptr),
		pIntradailyModulation(nullptr),
		pReservoirCapacity(nullptr),
		pLabelReservoirCapacity(nullptr),
		pReservoirManagement(nullptr)
	{
		OnStudyClosed.connect(this, &Prepro::onStudyClosed);
		if (notifier)
			notifier->onAreaChanged.connect(this, &Prepro::onAreaChanged);
	}


	void Prepro::createComponents()
	{
		if (pComponentsAreReady)
			return;
		pComponentsAreReady = true;

		{
			wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
			SetSizer(sizer);
			pSupport = new Component::Panel(this);
			sizer->Add(pSupport, 1, wxALL|wxEXPAND);
		}
		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		pSupport->SetSizer(sizer);

		const wxSize ourDefaultSize(55, wxDefaultSize.GetHeight());

		enum
		{
			right = wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL,
			left  = wxLEFT  | wxALIGN_LEFT  | wxALIGN_CENTER_VERTICAL,
		};

		auto* pGrid = new wxFlexGridSizer(5, 0, 8);
		sizer->Add(pGrid, 0, wxALL, 15);

		// Options
		{
			pGrid->Add(Component::CreateLabel(pSupport, wxT("Options"), false, true), 0, right);
		}
		// Intermonthly breakdown
		{
			pGrid->Add(Component::CreateLabel(pSupport, wxT("    Inter-monthly breakdown")), 0, right);

			auto* edit = new wxTextCtrl(pSupport, wxID_ANY, wxT("0.0"),
				wxDefaultPosition, ourDefaultSize, 0, Toolbox::Validator::Numeric());
			pGrid->Add(edit, 0, wxALL|wxEXPAND);
			pIntermonthlyBreakdown = edit;
		}
		// Intermonthly correlation
		{
			pGrid->Add(Component::CreateLabel(pSupport, wxT("  Inter-monthly correlation")), 0, right);

			auto* edit = new wxTextCtrl(pSupport, wxID_ANY, wxT("0.0"),
				wxDefaultPosition, ourDefaultSize, 0, Toolbox::Validator::Numeric());
			pGrid->Add(edit, 0, wxALL|wxEXPAND);
			pIntermonthlyCorrelation = edit;
		}
		// empty
		{
			pGrid->AddSpacer(10);
		}
		// Interdaily breakdown
		{
			pGrid->Add(Component::CreateLabel(pSupport, wxT("Inter-daily breakdown")), 0, right);

			auto* edit = new wxTextCtrl(pSupport, wxID_ANY, wxT("0.0"),
				wxDefaultPosition, ourDefaultSize, 0, Toolbox::Validator::Numeric());
			pGrid->Add(edit, 0, wxALL|wxEXPAND);
			pInterdailyBreakdown = edit;
		}
		// Interdaily modulation
		{
			pGrid->Add(Component::CreateLabel(pSupport, wxT("Intra-daily modulation")), 0, right);

			auto* edit = new wxTextCtrl(pSupport, wxID_ANY, wxT("0.0"),
				wxDefaultPosition, ourDefaultSize, 0, Toolbox::Validator::Numeric());
			pGrid->Add(edit, 0, wxALL|wxEXPAND);
			pIntradailyModulation = edit;
		}
		// Space
		{
			enum { verticalSpace = 10 };
			pGrid->AddSpacer(verticalSpace);
			pGrid->AddSpacer(verticalSpace);
			pGrid->AddSpacer(verticalSpace);
			pGrid->AddSpacer(verticalSpace);
			pGrid->AddSpacer(verticalSpace);
		}
		// empty
		{
			pGrid->Add(Component::CreateLabel(pSupport, wxT("   Reservoir"), false, true), 0, right);
		}
		// Reservoir management
		{
			pGrid->Add(Component::CreateLabel(pSupport, wxT("Reservoir management")), 0, right);

			auto* button = new Component::Button(pSupport, wxT("Yes"));
			button->menu(true);
			button->onPopupMenu(this, &Prepro::onToggleReservoirManagement);
			pGrid->Add(button, 0, left);
			pReservoirManagement = button;
		}
		// Reservoir capacity
		{
			auto* label = Component::CreateLabel(pSupport, wxT("Reservoir capacity (GWh)"));
			pLabelReservoirCapacity = label;
			pGrid->Add(label, 0, right);

			auto* edit = new wxTextCtrl(pSupport, wxID_ANY, wxT("0.0"),
				wxDefaultPosition, ourDefaultSize, 0, Toolbox::Validator::Numeric());
			pGrid->Add(edit, 0, wxALL|wxEXPAND);
			pReservoirCapacity = edit;
		}

		pGrid->SetItemMinSize(pIntermonthlyBreakdown, 64, 10);
		pGrid->SetItemMinSize(pReservoirCapacity, 64, 10);


		wxBoxSizer* ssGrids = new wxBoxSizer(wxHORIZONTAL);
		ssGrids->Add(new Component::Datagrid::Component(pSupport,
			new Component::Datagrid::Renderer::HydroPrepro(this, pInputAreaSelector), wxT("Overall monthly hydro Energy (ROR + Storage)")),
			4, wxALL|wxEXPAND, 5);

		ssGrids->Add(new wxStaticLine(pSupport, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), 0, wxALL|wxEXPAND);

		ssGrids->Add(new Component::Datagrid::Component(pSupport,
			new Component::Datagrid::Renderer::HydroMonthlyPower(this, pInputAreaSelector), wxT("Hydro storage daily maximum Power")),
			3, wxALL|wxEXPAND, 5);

		sizer->Add(ssGrids, 1, wxALL|wxEXPAND);
		sizer->Layout();


		pInterdailyBreakdown->Connect(pInterdailyBreakdown->GetId(), wxEVT_COMMAND_TEXT_UPDATED,
			wxCommandEventHandler(Prepro::onInterdailyBreakdownChanged), nullptr, this);
		pIntradailyModulation->Connect(pIntradailyModulation->GetId(), wxEVT_COMMAND_TEXT_UPDATED,
			wxCommandEventHandler(Prepro::onIntradailyModulationChanged), nullptr, this);

		pIntermonthlyCorrelation->Connect(pIntermonthlyCorrelation->GetId(), wxEVT_COMMAND_TEXT_UPDATED,
			wxCommandEventHandler(Prepro::onIntermonthlyCorrelationChanged), nullptr, this);
		pIntermonthlyBreakdown->Connect(pIntermonthlyBreakdown->GetId(), wxEVT_COMMAND_TEXT_UPDATED,
			wxCommandEventHandler(Prepro::onIntermonthlyBreakdownChanged), nullptr, this);

		pReservoirCapacity->Connect(pReservoirCapacity->GetId(), wxEVT_COMMAND_TEXT_UPDATED,
			wxCommandEventHandler(Prepro::onReservoirCapacityChanged), nullptr, this);
	}


	Prepro::~Prepro()
	{
		destroyBoundEvents();
		// destroy all children as soon as possible to prevent against corrupt vtable
		DestroyChildren();
	}



	void Prepro::onAreaChanged(Data::Area* area)
	{
		pArea = area;
		if (area and area->hydro.prepro)
		{
			// create components on-demand
			if (!pComponentsAreReady)
				createComponents();
			else
				GetSizer()->Show(pSupport, true);

			pIntermonthlyBreakdown->ChangeValue(wxString()  << area->hydro.intermonthlyBreakdown);
			pIntermonthlyCorrelation->ChangeValue(wxString()<< area->hydro.prepro->intermonthlyCorrelation);
			pInterdailyBreakdown->ChangeValue(wxString()    << area->hydro.interDailyBreakdown);
			pIntradailyModulation->ChangeValue(wxString()   << area->hydro.intraDailyModulation);
			pReservoirCapacity->ChangeValue(wxString()      << (area->hydro.reservoirCapacity / 1000.));
			if (area->hydro.reservoirManagement)
			{
				pReservoirManagement->caption(wxT("Yes"));
				pReservoirManagement->image("images/16x16/light_green.png");
				pLabelReservoirCapacity->Enable(true);
			}
			else
			{
				pReservoirManagement->caption(wxT("No"));
				pReservoirManagement->image("images/16x16/light_orange.png");
				pLabelReservoirCapacity->Enable(false);
			}
		}
		else
		{
			if (pComponentsAreReady)
			{
				pIntermonthlyBreakdown->ChangeValue(wxString(wxT("0.0")));
				pIntermonthlyCorrelation->ChangeValue(wxString(wxT("0.0")));
				pInterdailyBreakdown->ChangeValue(wxString(wxT("0.0")));
				pIntradailyModulation->ChangeValue(wxString(wxT("0.0")));
				pReservoirCapacity->ChangeValue(wxString(wxT("0")));
				pReservoirManagement->caption(wxT("No"));
				pReservoirManagement->image("images/16x16/light_orange.png");
				pLabelReservoirCapacity->Enable(false);
			}
		}
	}


	void Prepro::onIntermonthlyCorrelationChanged(wxCommandEvent& evt)
	{
		if (pArea)
		{
			if (evt.GetString().empty())
				return;
			double d;
			evt.GetString().ToDouble(&d);
			if (not Math::Equals(d, pArea->hydro.prepro->intermonthlyCorrelation))
			{
				if (d < 0.)
				{
					d = 0.;
					pIntermonthlyCorrelation->ChangeValue(wxT("0.0"));
				}
				pArea->hydro.prepro->intermonthlyCorrelation = d;
				MarkTheStudyAsModified();
			}
		}
	}


	void Prepro::onIntermonthlyBreakdownChanged(wxCommandEvent& evt)
	{
		if (pArea)
		{
			if (evt.GetString().empty())
				return;
			double d;
			evt.GetString().ToDouble(&d);
			if (not Math::Equals(d, pArea->hydro.intermonthlyBreakdown))
			{
				if (d < 0.)
				{
					d = 0.;
					pIntermonthlyBreakdown->ChangeValue(wxT("0.0"));
				}
				pArea->hydro.intermonthlyBreakdown = d;
				MarkTheStudyAsModified();
			}
		}
	}


	void Prepro::onInterdailyBreakdownChanged(wxCommandEvent& evt)
	{
		if (pArea)
		{
			if (evt.GetString().empty())
				return;
			double d;
			evt.GetString().ToDouble(&d);
			if (not Math::Equals(d, pArea->hydro.interDailyBreakdown))
			{
				pArea->hydro.interDailyBreakdown = d;
				MarkTheStudyAsModified();
			}
		}
	}


	void Prepro::onIntradailyModulationChanged(wxCommandEvent& evt)
	{
		if (pArea)
		{
			if (evt.GetString().empty())
				return;
			double d;
			evt.GetString().ToDouble(&d);
			if (not Math::Equals(d, pArea->hydro.intraDailyModulation))
			{
				if (d < 1.)
				{
					d = 1.;
					pIntradailyModulation->ChangeValue(wxT("1"));
				}

				pArea->hydro.intraDailyModulation = d;
				MarkTheStudyAsModified();
			}
		}
	}


	void Prepro::onReservoirCapacityChanged(wxCommandEvent& evt)
	{
		if (pArea)
		{
			if (evt.GetString().empty())
				return;
			double d;
			evt.GetString().ToDouble(&d);
			d *= 1000.;
			if (d < 1e-6)
				d = 0;
			if (not Math::Equals(d, pArea->hydro.reservoirCapacity))
			{
				pArea->hydro.reservoirCapacity = d;
				MarkTheStudyAsModified();
			}
		}
	}


	void Prepro::onStudyClosed()
	{
		pArea = nullptr;

		if (GetSizer())
			GetSizer()->Show(pSupport, false);
	}



	void Prepro::onToggleReservoirManagement(Component::Button&, wxMenu& menu, void*)
	{
		if (!pArea)
		{
			Menu::CreateItem(&menu, wxID_ANY, wxT("Please select an area"),
				"images/16x16/light_orange.png", wxEmptyString);
			return;
		}
		wxMenuItem* it;

		it = Menu::CreateItem(&menu, wxID_ANY, wxT("Use reservoir management"),
			"images/16x16/light_green.png", wxEmptyString);
		menu.Connect(it->GetId(), wxEVT_COMMAND_MENU_SELECTED,
			wxCommandEventHandler(Prepro::onEnableReserveManagement), nullptr, this);
		it = Menu::CreateItem(&menu, wxID_ANY, wxT("No"), "images/16x16/light_orange.png", wxEmptyString);
		menu.Connect(it->GetId(), wxEVT_COMMAND_MENU_SELECTED,
			wxCommandEventHandler(Prepro::onDisableReserveManagement), nullptr, this);
	}


	void Prepro::onEnableReserveManagement(wxCommandEvent&)
	{
		if (pArea and not pArea->hydro.reservoirManagement)
		{
			pArea->hydro.reservoirManagement = true;
			MarkTheStudyAsModified();
			onAreaChanged(pArea);
		}
	}


	void Prepro::onDisableReserveManagement(wxCommandEvent&)
	{
		if (pArea and pArea->hydro.reservoirManagement)
		{
			pArea->hydro.reservoirManagement = false;
			MarkTheStudyAsModified();
			onAreaChanged(pArea);
		}
	}






} // namespace Hydro
} // namespace Window
} // namespace Antares
