/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "dailypower.h"
#include "../../toolbox/components/datagrid/renderer/area/hydroprepro.h"
#include "../../toolbox/components/datagrid/renderer/area/inflowpattern.h"
#include "../../toolbox/components/datagrid/renderer/area/hydromonthlypower.h"
#include "../../toolbox/components/datagrid/renderer/area/creditmodulations.h"
#include "../../toolbox/validator.h"
#include "../../toolbox/create.h"
#include <wx/statline.h>

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace Hydro
{
Dailypower::Dailypower(wxWindow* parent, Toolbox::InputSelector::Area* notifier) :
 Component::Panel(parent),
 pInputAreaSelector(notifier),
 pArea(nullptr),
 pComponentsAreReady(false),
 pSupport(nullptr)
{
    OnStudyClosed.connect(this, &Dailypower::onStudyClosed);
    if (notifier)
        notifier->onAreaChanged.connect(this, &Dailypower::onAreaChanged);
}

void Dailypower::createComponents()
{
    if (pComponentsAreReady)
        return;
    pComponentsAreReady = true;

    {
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        SetSizer(sizer);
        pSupport = new Component::Panel(this);
        sizer->Add(pSupport, 1, wxALL | wxEXPAND);
    }

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    pSupport->SetSizer(sizer);

    wxBoxSizer* sizerHigh = new wxBoxSizer(wxHORIZONTAL);

    sizer->Add(sizerHigh, 2, wxALL | wxEXPAND | wxFIXED_MINSIZE);

    sizerHigh->Add(new Component::Datagrid::Component(
                     pSupport,
                     new Component::Datagrid::Renderer::CreditModulations(this, pInputAreaSelector),
                     wxT("Credit Modulations (reservoir level)")),
                   3,
                   wxALL | wxEXPAND,
                   5);
    sizerHigh->SetMinSize(10, 200);

    wxBoxSizer* ssGridsLow = new wxBoxSizer(wxHORIZONTAL);

    ssGridsLow->Add(new Component::Datagrid::Component(
                      pSupport,
                      new Component::Datagrid::Renderer::HydroMonthlyHours(
                        this,
                        pInputAreaSelector,
                        Component::Datagrid::Renderer::HydroMonthlyHours::HoursType::Generation),
                      wxT("Standard Credits Calendar (Maximum Generation)")),
                    3,
                    wxALL | wxEXPAND,
                    5);

    sizer->Add(ssGridsLow, 4, wxALL | wxEXPAND | wxFIXED_MINSIZE);

    ssGridsLow->Add(new Component::Datagrid::Component(
                      pSupport,
                      new Component::Datagrid::Renderer::HydroMonthlyHours(
                        this,
                        pInputAreaSelector,
                        Component::Datagrid::Renderer::HydroMonthlyHours::HoursType::Pumping),
                      wxT("Standard Credits Calendar (Maximum Pumping)")),
                    3,
                    wxALL | wxEXPAND,
                    5);

    sizer->Layout();
}

Dailypower::~Dailypower()
{
    destroyBoundEvents();
    // destroy all children as soon as possible to prevent against corrupt vtable
    DestroyChildren();
}

void Dailypower::onAreaChanged(Data::Area* area)
{
    pArea = area;
    if (area and area->hydro.prepro)
    {
        // create components on-demand
        if (!pComponentsAreReady)
            createComponents();
        else
            GetSizer()->Show(pSupport, true);
    }
}

void Dailypower::onStudyClosed()
{
    pArea = nullptr;

    if (GetSizer())
        GetSizer()->Show(pSupport, false);
}

} // namespace Hydro
} // namespace Window
} // namespace Antares
