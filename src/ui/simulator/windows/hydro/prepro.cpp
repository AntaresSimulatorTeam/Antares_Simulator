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
#include "../../toolbox/components/datagrid/renderer/area/inflowpattern.h"
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
		pIntermonthlyCorrelation(nullptr)
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


		wxBoxSizer* sizerH = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(sizerH, 0, wxALL | wxEXPAND | wxFIXED_MINSIZE, 15);
		// Options
		{
			sizerH->Add(Component::CreateLabel(pSupport, wxT("Options"), false, true), 0, right);
		}
		// Intermonthly correlation
		{
			sizerH->Add(Component::CreateLabel(pSupport, wxT("  Inter-monthly correlation")), 0, right);

			auto* edit = new wxTextCtrl(pSupport, wxID_ANY, wxT("0.0"),
				wxDefaultPosition, ourDefaultSize, 0, Toolbox::Validator::Numeric());
			sizerH->Add(edit, 0, wxALL| wxFIXED_MINSIZE);
			pIntermonthlyCorrelation = edit;
		}

		wxBoxSizer* ssGrids = new wxBoxSizer(wxHORIZONTAL);
		ssGrids->Add(new Component::Datagrid::Component(pSupport,
			new Component::Datagrid::Renderer::HydroPrepro(this, pInputAreaSelector), wxT("Overall monthly hydro Energy (ROR + Storage)")),
			4, wxALL|wxEXPAND, 5);

		ssGrids->Add(new wxStaticLine(pSupport, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL), 0, wxALL | wxEXPAND);

		ssGrids->Add(new Component::Datagrid::Component(pSupport,
			new Component::Datagrid::Renderer::InflowPattern(this, pInputAreaSelector), wxT("Inflow Pattern")),
			2, wxALL | wxEXPAND, 5);


		sizer->Add(ssGrids, 1, wxALL|wxEXPAND);
		sizer->Layout();
		pIntermonthlyCorrelation->Connect(pIntermonthlyCorrelation->GetId(), wxEVT_COMMAND_TEXT_UPDATED,
			wxCommandEventHandler(Prepro::onIntermonthlyCorrelationChanged), nullptr, this);
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

			pIntermonthlyCorrelation->ChangeValue(wxString()<< area->hydro.prepro->intermonthlyCorrelation);
		}
		else
		{
			if (pComponentsAreReady)
			{
				pIntermonthlyCorrelation->ChangeValue(wxString(wxT("0.0")));
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



	void Prepro::onStudyClosed()
	{
		pArea = nullptr;

		if (GetSizer())
			GetSizer()->Show(pSupport, false);
	}








} // namespace Hydro
} // namespace Window
} // namespace Antares
