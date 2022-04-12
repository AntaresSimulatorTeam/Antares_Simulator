
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

namespace Antares
{
namespace Window
{
namespace Options
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
    auto study = Data::Study::Current::Get();

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

AdequacyPatchAreas::~AdequacyPatchAreas()
{
}

void AdequacyPatchAreas::onClose(void*)
{
    Dispatcher::GUI::Close(this);
}

void AdequacyPatchAreas::mouseMoved(wxMouseEvent&)
{
    // Notify other components as well
    Antares::Component::Panel::OnMouseMoveFromExternalComponent();
}

} // namespace Options
} // namespace Window
} // namespace Antares
