/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "select-output.h"
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
#include "../../../toolbox/components/datagrid/renderer/select-variables.h"
#include "../../message.h"

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace Options
{
BEGIN_EVENT_TABLE(SelectOutput, wxDialog)
EVT_MOTION(SelectOutput::mouseMoved)
EVT_CHECKBOX(mnIDEnabled, SelectOutput::evtEnabled)
END_EVENT_TABLE()

SelectOutput::SelectOutput(wxFrame* parent) :
 wxDialog(parent,
          wxID_ANY,
          wxT("Output Selection"),
          wxDefaultPosition,
          wxSize(380, 410),
          wxCLOSE_BOX | wxCAPTION | wxRESIZE_BORDER)
{
    assert(parent);

    // The current study
    auto study = GetCurrentStudy();

    // Title of the Form
    SetLabel(wxT("Output Selection"));
    SetTitle(GetLabel());

    auto* sizer = new wxBoxSizer(wxVERTICAL);

    // Header
    sizer->Add(
      Toolbox::Components::WizardHeader::Create(
        this, wxT("Options"), "images/32x32/orderedlist.png", wxT("Configure output selection")),
      0,
      wxALL | wxEXPAND | wxFIXED_MINSIZE);

    sizer->AddSpacer(20);

    {
        wxSizer* hz = new wxBoxSizer(wxHORIZONTAL);
        pStatus = new wxCheckBox(this, mnIDEnabled, wxT(" Ask for selecting output variables "));
        hz->Add(24, 5);
        hz->Add(pStatus, 0, wxALL | wxEXPAND);
        hz->Add(20, 5);
        sizer->Add(hz, 0, wxALL | wxEXPAND);
        sizer->AddSpacer(15);
    }

    {
        wxString s = wxT("All output variables will be saved on disk  (");
        if (!(!study))
        {
            uint nvars = study->parameters.variablesPrintInfo.size();
            s << nvars << wxT(" variables)");
        }
        else
            s << wxT("no study)");

        pInfo = Component::CreateLabel(this, s, false, true);
        sizer->Add(pInfo, 1, wxLEFT | wxRIGHT | wxEXPAND | wxALIGN_TOP, 25);
    }

    // Panel
    pPanel = new Component::Panel(this);
    sizer->Add(pPanel, 1, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN);

    wxSizer* hz = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);
    wxButton* btn;

    // Select all
    btn = Component::CreateButton(pPanel, wxT("Enable all"), this, &SelectOutput::onSelectAll);
    rightSizer->Add(btn, 0, wxEXPAND | wxLEFT | wxRIGHT);

    // Unselect all
    rightSizer->AddSpacer(2);
    btn = Component::CreateButton(pPanel, wxT("Disable all"), this, &SelectOutput::onUnselectAll);
    rightSizer->Add(btn, 0, wxEXPAND | wxLEFT | wxRIGHT);

    // Unselect all
    rightSizer->AddSpacer(10);
    btn = Component::CreateButton(pPanel, wxT("Reverse"), this, &SelectOutput::onToggle);
    rightSizer->Add(btn, 0, wxEXPAND | wxLEFT | wxRIGHT);

    // Datagrid
    auto* renderer = new Component::Datagrid::Renderer::SelectVariables();
    renderer->study = study;
    renderer->onTriggerUpdate.bind(this, &SelectOutput::updateCaption);
    auto* grid
      = new Component::Datagrid::Component(pPanel, renderer, wxEmptyString, false, true, true);
    grid->SetBackgroundColour(GetBackgroundColour());
    pGrid = grid;
    renderer->control(grid);

    hz->AddSpacer(13);
    hz->Add(grid, 1, wxALL | wxEXPAND);

    hz->AddSpacer(10);
    Component::AddVerticalSeparator(pPanel, hz, 1, 0);
    hz->AddSpacer(10);

    hz->Add(rightSizer, 0, wxALL | wxEXPAND);
    hz->AddSpacer(20);
    pPanel->SetSizer(hz);
    sizer->AddSpacer(5);

    sizer->Add(new wxStaticLine(this, wxID_ANY), 0, wxALL | wxEXPAND, 8);

    hz = new wxBoxSizer(wxHORIZONTAL);
    btn = Component::CreateButton(this, wxT("  Close  "), this, &SelectOutput::onClose);
    hz->AddStretchSpacer();
    hz->Add(btn, 0, wxEXPAND | wxALL);
    hz->Add(20, 5);
    sizer->Add(hz, 0, wxALL | wxEXPAND);
    sizer->AddSpacer(8);

    sizer->Layout();
    SetSizer(sizer);

    // Recentre the window
    Centre(wxBOTH);

    SetAutoLayout(true);

    onUpdateStatus();
    grid->forceRefresh();
}

SelectOutput::~SelectOutput()
{
}

void SelectOutput::onClose(void*)
{
    Dispatcher::GUI::Close(this);
}

void SelectOutput::onSelectAll(void*)
{
    auto studyptr = GetCurrentStudy();
    if (!studyptr)
        return;
    auto& study = *studyptr;

    Freeze();
    study.parameters.variablesPrintInfo.setAllPrintStatusesTo(true);
    pGrid->forceRefresh();
    updateCaption();
    Dispatcher::GUI::Refresh(pGrid);
    MarkTheStudyAsModified();
    Thaw();
}

void SelectOutput::onUnselectAll(void*)
{
    auto studyptr = GetCurrentStudy();
    if (!studyptr)
        return;
    auto& study = *studyptr;

    Freeze();
    study.parameters.variablesPrintInfo.setAllPrintStatusesTo(false);
    pGrid->forceRefresh();
    updateCaption();
    Dispatcher::GUI::Refresh(pGrid);
    MarkTheStudyAsModified();
    Thaw();
}

void SelectOutput::onToggle(void*)
{
    auto studyptr = GetCurrentStudy();
    if (!studyptr)
        return;
    auto& study = *studyptr;

    Freeze();
    study.parameters.variablesPrintInfo.reverseAll();
    pGrid->forceRefresh();
    updateCaption();
    Dispatcher::GUI::Refresh(pGrid);
    MarkTheStudyAsModified();
    Thaw();
}

void SelectOutput::mouseMoved(wxMouseEvent&)
{
    // Notify other components as well
    Antares::Component::Panel::OnMouseMoveFromExternalComponent();
}

void SelectOutput::updateCaption()
{
    auto studyptr = GetCurrentStudy();
    if (!studyptr)
        return;
    auto& study = *studyptr;

    auto& d = study.parameters;

    if (d.thematicTrimming)
    {
        uint nbPrintedVars = d.variablesPrintInfo.numberOfEnabledVariables();
        if (nbPrintedVars < 2)
            pStatus->SetLabel(wxString()
                              << wxT(" Ask for selecting ") << nbPrintedVars << wxT(" output variable  "));
        else
            pStatus->SetLabel(wxString()
                              << wxT(" Ask for selecting ") << nbPrintedVars << wxT(" output variables  "));
    }
    else
        pStatus->SetLabel(wxT(" Ask for selecting output variables "));
}

void SelectOutput::onUpdateStatus()
{
    auto studyptr = GetCurrentStudy();
    if (!studyptr)
        return;
    auto& study = *studyptr;

    Freeze();
    bool b = study.parameters.thematicTrimming;

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

void SelectOutput::evtEnabled(wxCommandEvent& evt)
{
    auto studyptr = GetCurrentStudy();

    Freeze();
    if (!(!studyptr))
    {
        bool v = evt.IsChecked();
        auto& d = studyptr->parameters;
        d.thematicTrimming = v;
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
