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

#include "playlist.h"
#include <wx/sizer.h>
#include <wx/settings.h>
#include <wx/statline.h>

#include <antares/study/finder.h>
#include "../../../toolbox/resources.h"
#include "../../../application/study.h"
#include "../../../application/main.h"
#include "../../../toolbox/resources.h"
#include "../../../toolbox/create.h"
#include "../../../application/wait.h"
#include "../../../toolbox/components/wizardheader.h"
#include "../../../toolbox/components/datagrid/renderer/mc-playlist.h"
#include "../../message.h"

using namespace Yuni;



namespace Antares
{
namespace Window
{
namespace Options
{

	BEGIN_EVENT_TABLE(MCPlaylist, wxDialog)
		EVT_MOTION(MCPlaylist::mouseMoved)
		EVT_CHECKBOX(mnIDEnabled, MCPlaylist::evtEnabled)
	END_EVENT_TABLE()





	MCPlaylist::MCPlaylist(wxFrame* parent) :
		wxDialog(parent, wxID_ANY, wxT("MC Playlist"), wxDefaultPosition, wxSize(380,410),
			wxCLOSE_BOX|wxCAPTION)
	{
		assert(parent);

		// The current study
		auto study = Data::Study::Current::Get();

		// Title of the Form
		SetLabel(wxT("MC Playlist"));
		SetTitle(GetLabel());

		auto* sizer = new wxBoxSizer(wxVERTICAL);

		// Header
		sizer->Add(Toolbox::Components::WizardHeader::Create(this, wxT("Options"),
			"images/32x32/orderedlist.png", wxT("Configure MC playlist")), 0, wxALL|wxEXPAND | wxFIXED_MINSIZE);

		sizer->AddSpacer(20);

		{
			wxSizer* hz = new wxBoxSizer(wxHORIZONTAL);
			pStatus = new wxCheckBox(this, mnIDEnabled, wxT(" Use a custom playlist  "));
			hz->Add(24, 5);
			hz->Add(pStatus, 0, wxALL|wxEXPAND);
			hz->Add(20, 5);
			sizer->Add(hz, 0, wxALL|wxEXPAND);
			sizer->AddSpacer(15);
		}

		{
			wxString s = wxT("All MC years of the simulation will be performed  (");
			if (!(!study))
			{
				uint y = study->parameters.nbYears;
				if (y < 2)
					s << wxT("1 year)");
				else
					s << y << wxT(" years)");
			}
			else
				s << wxT("no study)");

			pInfo = Component::CreateLabel(this, s, false, true);
			sizer->Add(pInfo, 1, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_TOP, 25);
		}


		// Panel
		pPanel = new Component::Panel(this);
		sizer->Add(pPanel, 1, wxALL|wxEXPAND|wxRESERVE_SPACE_EVEN_IF_HIDDEN);


		wxSizer* hz = new wxBoxSizer(wxHORIZONTAL);
		wxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);
		wxButton* btn;

		// Select all
		btn = Component::CreateButton(pPanel, wxT("Enable all"), this, &MCPlaylist::onSelectAll);
		rightSizer->Add(btn, 0, wxEXPAND|wxLEFT|wxRIGHT);

		// Unselect all
		rightSizer->AddSpacer(2);
		btn = Component::CreateButton(pPanel, wxT("Disable all"), this, &MCPlaylist::onUnselectAll);
		rightSizer->Add(btn, 0, wxEXPAND|wxLEFT|wxRIGHT);

		// Reverse status
		rightSizer->AddSpacer(10);
		btn = Component::CreateButton(pPanel, wxT("Reverse"), this, &MCPlaylist::onToggle);
		rightSizer->Add(btn, 0, wxEXPAND|wxLEFT|wxRIGHT);

        // Reset weights
        rightSizer->AddSpacer(10);
        btn = Component::CreateButton(pPanel, wxT("Reset weights"), this, &MCPlaylist::onResetYearsWeight);
        rightSizer->Add(btn, 0, wxEXPAND|wxLEFT|wxRIGHT);

		// Datagrid
		auto* renderer = new Component::Datagrid::Renderer::MCPlaylist();
		renderer->study = study;
		renderer->onTriggerUpdate.bind(this, &MCPlaylist::updateCaption);
		auto* grid = new Component::Datagrid::Component(pPanel, renderer, wxEmptyString, false, true, true);
		grid->SetBackgroundColour(GetBackgroundColour());
		pGrid = grid;
		renderer->control(grid);

		hz->AddSpacer(13);
		hz->Add(grid, 1, wxALL|wxEXPAND);

		hz->AddSpacer(10);
		Component::AddVerticalSeparator(pPanel, hz, 1, 0);
		hz->AddSpacer(10);

		hz->Add(rightSizer, 0, wxALL|wxEXPAND);
		hz->AddSpacer(20);
		pPanel->SetSizer(hz);
		sizer->AddSpacer(5);

		sizer->Add(new wxStaticLine(this, wxID_ANY), 0, wxALL|wxEXPAND, 8);


		hz = new wxBoxSizer(wxHORIZONTAL);
		btn = Component::CreateButton(this, wxT("  Close  "), this, &MCPlaylist::onClose);
		hz->AddStretchSpacer();
		hz->Add(btn, 0, wxEXPAND|wxALL);
		hz->Add(20, 5);
		sizer->Add(hz, 0, wxALL|wxEXPAND);
		sizer->AddSpacer(8);


		sizer->Layout();
		SetSizer(sizer);

		// Recentre the window
		Centre(wxBOTH);

		SetAutoLayout(true);

		onUpdateStatus();
		grid->forceRefresh();
	}


	MCPlaylist::~MCPlaylist()
	{
	}




	void MCPlaylist::onClose(void*)
	{
		Dispatcher::GUI::Close(this);
	}


	void MCPlaylist::onSelectAll(void*)
	{
		auto studyptr = Data::Study::Current::Get();
		if (!studyptr)
			return;
		auto& study = *studyptr;

		Freeze();
		for (uint i = 0; i != study.parameters.nbYears; ++i)
			study.parameters.yearsFilter[i] = true;
		pGrid->forceRefresh();
		updateCaption();
		Dispatcher::GUI::Refresh(pGrid);
		MarkTheStudyAsModified();
		Thaw();
	}


	void MCPlaylist::onUnselectAll(void*)
	{
		auto studyptr = Data::Study::Current::Get();
		if (!studyptr)
			return;
		auto& study = *studyptr;

		Freeze();
		for (uint i = 0; i != study.parameters.nbYears; ++i)
			study.parameters.yearsFilter[i] = false;
		pGrid->forceRefresh();
		updateCaption();
		Dispatcher::GUI::Refresh(pGrid);
		MarkTheStudyAsModified();
		Thaw();
	}


	void MCPlaylist::onToggle(void*)
	{
		auto studyptr = Data::Study::Current::Get();
		if (!studyptr)
			return;
		auto& study = *studyptr;

		Freeze();
		for (uint i = 0; i != study.parameters.nbYears; ++i)
		{
			study.parameters.yearsFilter[i] =
				!study.parameters.yearsFilter[i];
		}
		pGrid->forceRefresh();
		updateCaption();
		Dispatcher::GUI::Refresh(pGrid);
		MarkTheStudyAsModified();
		Thaw();
	}

    void MCPlaylist::onResetYearsWeight(void*)
    {
        auto studyptr = Data::Study::Current::Get();
        if (!studyptr)
            return;
        auto& study = *studyptr;
        
        study.parameters.resetYearsWeigth();
		onUpdateStatus();
    }

	void MCPlaylist::mouseMoved(wxMouseEvent&)
	{
		// Notify other components as well
		Antares::Component::Panel::OnMouseMoveFromExternalComponent();
	}


	void MCPlaylist::updateCaption()
	{
		auto studyptr = Data::Study::Current::Get();
		if (!studyptr)
			return;
		auto& study = *studyptr;

		auto& d = study.parameters;
		bool b = d.userPlaylist and d.nbYears > 1;

		if (b)
		{
			//Check if weight is !=1 for one year
			std::vector<float> yearWeight = d.getYearsWeight();
		    
		    int nbYearsDifferentFrom1 = 0;
			uint y = 0;
			for (uint i = 0; i != d.nbYears; ++i)
			{
				if (d.yearsFilter[i])
				{
                    ++y;

					float weight = yearWeight[i];
                    if (weight != 1) {
                        nbYearsDifferentFrom1++;
                    }
                }
			}

			wxString yearWeightLabel;

			if (nbYearsDifferentFrom1 == 1)
			{
				yearWeightLabel = wxString("\n(") << nbYearsDifferentFrom1 << " year with custom weight)";
			}
			if (nbYearsDifferentFrom1 > 1)
			{
				yearWeightLabel = wxString("\n(") << nbYearsDifferentFrom1 << " years with custom weight)";
			}

			if (y < 2)
				pStatus->SetLabel(wxString() << wxT(" Use a custom playlist with ") << y << wxT(" year  ") << yearWeightLabel);
			else
				pStatus->SetLabel(wxString() << wxT(" Use a custom playlist with ") << y << wxT(" years  ") << yearWeightLabel);

		}
		else
			pStatus->SetLabel(wxT(" Use a custom playlist  "));

		wxSizer& sizer = *GetSizer();
		sizer.Layout();
	}


	void MCPlaylist::onUpdateStatus()
	{
		auto studyptr = Data::Study::Current::Get();
		if (!studyptr)
			return;
		auto& study = *studyptr;

		Freeze();
		bool b = study.parameters.userPlaylist and study.parameters.nbYears > 1;

		updateCaption();
		wxSizer& sizer = *GetSizer();
		sizer.Show(pPanel, b);
		sizer.Show(pInfo, !b);
		pPanel->Show(b);
		pInfo->Show(!b);
		pStatus->SetValue(b);
		sizer.Layout();
		Thaw();
		Refresh();
	}


	void MCPlaylist::evtEnabled(wxCommandEvent& evt)
	{
		auto studyptr = Data::Study::Current::Get();

		Freeze();
		if (!(!studyptr))
		{
			bool v = evt.IsChecked();
			auto& d = studyptr->parameters;
			if (v and d.nbYears < 2)
			{
				Window::Message message(this, wxT("MC Playlist"), wxT("MC Playlist"),
					wxT("Impossible to enable the MC playlist with only 1 year"));
				message.add(Window::Message::btnContinue, true);
				message.showModal();
			}
			else
				d.userPlaylist = v;
		}
		onUpdateStatus();
		OnStudySimulationSettingsChanged();
		MarkTheStudyAsModified();
		Thaw();

		OnStudyUpdatePlaylist();
	}





} // namespace Options
} // namespace Window
} // namespace Antares
