// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "levelsandvalues.h"
#include <wx/stattext.h>
#include "../../toolbox/components/datagrid/renderer/area/reservoirlevels.h"
#include "../../toolbox/components/datagrid/renderer/area/watervalues.h"
#include "../../toolbox/components/button.h"
#include "../../toolbox/validator.h"
#include "../../toolbox/create.h"
#include "../../application/menus.h"
#include <wx/statline.h>

using namespace Yuni;

namespace Antares::Window::Hydro
{
LevelsAndValues::LevelsAndValues(wxWindow* parent, Toolbox::InputSelector::Area* notifier):
    Component::Panel(parent),
    pInputAreaSelector(notifier),
    pArea(nullptr),
    pComponentsAreReady(false),
    pSupport(nullptr)
{
    OnStudyClosed.connect(this, &LevelsAndValues::onStudyClosed);
    if (notifier)
    {
        notifier->onAreaChanged.connect(this, &LevelsAndValues::onAreaChanged);
    }
}

void LevelsAndValues::createComponents()
{
    if (pComponentsAreReady)
    {
        return;
    }
    pComponentsAreReady = true;

    {
        wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
        SetSizer(sizer);
        pSupport = new Component::Panel(this);
        sizer->Add(pSupport, 1, wxALL | wxEXPAND);
    }

    wxBoxSizer* ssGrids = new wxBoxSizer(wxHORIZONTAL);
    pSupport->SetSizer(ssGrids);

    ssGrids->Add(new Component::Datagrid::Component(
                   pSupport,
                   new Component::Datagrid::Renderer::ReservoirLevels(this, pInputAreaSelector),
                   wxT("Reservoir levels")),
                 2,
                 wxALL | wxEXPAND | wxFIXED_MINSIZE,
                 5);

    ssGrids->Add(
      new wxStaticLine(pSupport, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL),
      0,
      wxALL | wxEXPAND);

    ssGrids->Add(new Component::Datagrid::Component(
                   pSupport,
                   new Component::Datagrid::Renderer::WaterValues(this, pInputAreaSelector),
                   wxT("Water values")),
                 3,
                 wxALL | wxEXPAND | wxFIXED_MINSIZE,
                 5);

    ssGrids->Layout();
}

LevelsAndValues::~LevelsAndValues()
{
    destroyBoundEvents();
    // destroy all children as soon as possible to prevent against corrupt vtable
    DestroyChildren();
}

void LevelsAndValues::onAreaChanged(Data::Area* area)
{
    pArea = area;
    if (area and area->hydro.prepro)
    {
        // create components on-demand
        if (!pComponentsAreReady)
        {
            createComponents();
        }
        else
        {
            GetSizer()->Show(pSupport, true);
        }
    }
}

void LevelsAndValues::onStudyClosed()
{
    pArea = nullptr;

    if (GetSizer())
    {
        GetSizer()->Show(pSupport, false);
    }
}

} // namespace Antares::Window::Hydro
