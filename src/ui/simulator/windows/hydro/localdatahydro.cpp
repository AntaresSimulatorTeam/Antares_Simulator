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

#include "localdatahydro.h"
#include "prepro.h"
#include "dailypower.h"
#include "levelsandvalues.h"
#include "management.h"
#include "../../toolbox/components/datagrid/renderer/area/timeseries.h"
#include "../../toolbox/components/datagrid/renderer/area/hydromonthlypower.h"

using namespace Yuni;

namespace Antares::Window::Hydro
{
Localdatahydro::Localdatahydro(wxWindow* parent, Toolbox::InputSelector::Area* notifier) :
 Component::Panel(parent), pNotifier(notifier), pLastArea(nullptr)
{
    auto* notebook = new Component::Notebook(this, Component::Notebook::orTop);
    notebook->displayTitle(false);
    notebook->theme(Component::Notebook::themeLight);

    auto* page1 = new Antares::Window::Hydro::Management(notebook, notifier);
    pPageFatal = notebook->add(page1, wxT("Management Options"));

    auto* page2 = new Antares::Window::Hydro::Prepro(notebook, notifier);
    pPageFatal = notebook->add(page2, wxT("Inflow Structure"));

    auto* page3 = new Antares::Window::Hydro::Dailypower(notebook, notifier);
    pPageFatal = notebook->add(page3, wxT("Daily Power and Energy Credits"));

    auto* page4 = new Antares::Window::Hydro::LevelsAndValues(notebook, notifier);
    pPageFatal = notebook->add(page4, wxT("Reservoir levels and water values"));

    // Connection to the notifier
    if (pNotifier)
        pNotifier->onAreaChanged.connect(this, &Localdatahydro::onAreaChanged);
    OnStudyClosed.connect(this, &Localdatahydro::onStudyClosed);

    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(notebook, 1, wxALL | wxEXPAND);
    SetSizer(sizer);

    OnStudyLoaded.connect(this, &Localdatahydro::onStudyLoaded);
}

Localdatahydro::~Localdatahydro()
{
    destroyBoundEvents();
}

void Localdatahydro::onAreaChanged(Data::Area* area)
{
    pLastArea = CurrentStudyIsValid() ? area : nullptr;
}

void Localdatahydro::onStudyClosed()
{
    pLastArea = nullptr;
    if (pNotifier)
        pNotifier->onAreaChanged(nullptr);
}

void Localdatahydro::onStudyLoaded()
{
    if (pPageFatal)
        pPageFatal->select();
}

} // namespace Antares::Window::Hydro


