// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "group.h"
#include <wx/sizer.h>
#include <yuni/core/math.h>

using namespace Yuni;

namespace Antares::Component
{
void PanelGroup::SetDarkBackgroundColor(wxWindow* ctrl, int lightModifier)
{
    wxColour def = ctrl->GetBackgroundColour();
    int r = Math::MinMax<int>(def.Red() - 100 + lightModifier, 0, 255);
    int g = Math::MinMax<int>(def.Green() - 90 + lightModifier, 0, 255);
    int b = Math::MinMax<int>(def.Blue() - 70 + lightModifier, 0, 255);
    ctrl->SetBackgroundColour(wxColour(r, g, b));
}

void PanelGroup::SetLighterBackgroundColor(wxWindow* ctrl, int lightModifier)
{
    wxColour def = ctrl->GetBackgroundColour();
    int r = Math::MinMax<int>(def.Red() + 40 + lightModifier, 0, 255);
    int g = Math::MinMax<int>(def.Green() + 40 + lightModifier, 0, 255);
    int b = Math::MinMax<int>(def.Blue() + 40 + lightModifier, 0, 255);
    ctrl->SetBackgroundColour(wxColour(r, g, b));
}

PanelGroup::PanelGroup(wxWindow* parent, const char* /*image*/):
    Panel(parent)
{
    assert(parent);

    auto* hz = new wxBoxSizer(wxHORIZONTAL);
    subpanel = new Panel(this);
    hz->AddSpacer(3);
    leftSizer = new wxBoxSizer(wxHORIZONTAL);
    hz->Add(leftSizer, 0, wxALL | wxEXPAND, 1);
    hz->AddSpacer(3);
    hz->Add(subpanel, 1, wxALL | wxEXPAND, 1);
    SetSizer(hz);

    wxColour def = GetBackgroundColour();
    {
        int r = Math::MinMax<int>(def.Red() - 80, 0, 255);
        int g = Math::MinMax<int>(def.Green() - 70, 0, 255);
        int b = Math::MinMax<int>(def.Blue() - 50, 0, 255);
        SetBackgroundColour(wxColour(r, g, b));
    }
    {
        int r = Math::MinMax<int>(def.Red() - 5, 0, 255);
        int g = Math::MinMax<int>(def.Green() - 5, 0, 255);
        int b = Math::MinMax<int>(def.Blue() - 0, 0, 255);
        subpanel->SetBackgroundColour(wxColour(r, g, b));
    }

    hz = new wxBoxSizer(wxHORIZONTAL);
    hz->AddSpacer(1);
    subpanel->SetSizer(hz);
}

PanelGroup::~PanelGroup()
{
}

} // namespace Antares::Component
