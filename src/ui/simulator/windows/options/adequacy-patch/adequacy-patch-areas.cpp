/*
** Copyright 2007-2023 RTE
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

#include "adequacy-patch-areas.h"
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
#include "../../../toolbox/components/datagrid/renderer/adequacy-patch-area-grid.h"
#include "../../message.h"

using namespace Yuni;

namespace Antares::Window::Options
{
BEGIN_EVENT_TABLE(AdequacyPatchAreas, wxDialog)
EVT_MOTION(AdequacyPatchAreas::mouseMoved)
END_EVENT_TABLE()

AdequacyPatchAreas::AdequacyPatchAreas(wxFrame* parent) :
 wxDialog(parent,
          wxID_ANY,
          wxT("Adequacy Patch Areas"),
          wxDefaultPosition,
          wxSize(380, 410),
          wxCLOSE_BOX | wxCAPTION | wxRESIZE_BORDER)
{
    assert(parent);

    // The current study
    auto study = GetCurrentStudy();

    // Title of the Form
    SetLabel(wxT("Adequacy Patch Areas"));
    SetTitle(GetLabel());

    auto* sizer = new wxBoxSizer(wxVERTICAL);

    // Header
    sizer->Add(
      Toolbox::Components::WizardHeader::Create(this,
                                                wxT("Options"),
                                                "images/32x32/orderedlist.png",
                                                wxT("Configure areas mode for adequacy patch")),
      0,
      wxALL | wxEXPAND | wxFIXED_MINSIZE);

    sizer->AddSpacer(20);

    // Panel
    pPanel = new Component::Panel(this);
    sizer->Add(pPanel, 1, wxALL | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN);

    wxSizer* hz = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);
    wxButton* btn;

    // Datagrid
    auto* renderer = new Component::Datagrid::Renderer::AdequacyPatchAreaGrid();
    renderer->study = study;
    auto* grid
      = new Component::Datagrid::Component(pPanel, renderer, wxEmptyString, false, true, true);
    grid->SetBackgroundColour(GetBackgroundColour());
    pGrid = grid;
    renderer->control(grid);

    hz->AddSpacer(13);
    hz->Add(grid, 1, wxALL | wxEXPAND);

    hz->Add(rightSizer, 0, wxALL | wxEXPAND);
    hz->AddSpacer(20);
    pPanel->SetSizer(hz);
    sizer->AddSpacer(5);

    sizer->Add(new wxStaticLine(this, wxID_ANY), 0, wxALL | wxEXPAND, 8);

    hz = new wxBoxSizer(wxHORIZONTAL);
    btn = Component::CreateButton(this, wxT("  Close  "), this, &AdequacyPatchAreas::onClose);
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
    grid->forceRefresh();
}

AdequacyPatchAreas::~AdequacyPatchAreas() = default;

void AdequacyPatchAreas::onClose(const void*)
{
    Dispatcher::GUI::Close(this);
}

void AdequacyPatchAreas::mouseMoved(wxMouseEvent&)
{
    // Notify other components as well
    Antares::Component::Panel::OnMouseMoveFromExternalComponent();
}

}
