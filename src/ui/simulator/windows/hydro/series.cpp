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

#include "series.h"
#include "../../toolbox/components/notebook/notebook.h"
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/components/datagrid/renderer/area/timeseries.h"

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace Hydro
{
Series::Series(wxWindow* parent, Toolbox::InputSelector::Area* notifier) :
 Component::Panel(parent), pNotifier(notifier), pLastArea(nullptr)
{
    auto* notebook = new Component::Notebook(this, Component::Notebook::orTop);
    notebook->displayTitle(false);
    notebook->theme(Component::Notebook::themeLight);

    auto* com = new Component::Datagrid::Component(notebook);
    com->renderer(new Component::Datagrid::Renderer::TimeSeriesHydroFatal(com, notifier));
    pPageFatal = notebook->add(com, wxT("Run-of-the-river (ROR)"));

    com = new Component::Datagrid::Component(notebook);
    com->renderer(new Component::Datagrid::Renderer::TimeSeriesHydroMod(com, notifier));
    pPageFatal = notebook->add(com, wxT("Hydro Storage"));

    com = new Component::Datagrid::Component(notebook);
    com->renderer(new Component::Datagrid::Renderer::TimeSeriesHydroMinGen(com, notifier));
    pPageFatal = notebook->add(com, wxT("Minimum Generation"));

    com = new Component::Datagrid::Component(notebook);
    com->renderer(new Component::Datagrid::Renderer::TimeSeriesHydroMaxHourlyGenPower(com, notifier));
    pPageFatal = notebook->add(com, wxT("Maximum Generation"));

    com = new Component::Datagrid::Component(notebook);
    com->renderer(new Component::Datagrid::Renderer::TimeSeriesHydroMaxHourlyPumpPower(com, notifier));
    pPageFatal = notebook->add(com, wxT("Maximum Pumping"));

    // Connection to the notifier
    if (pNotifier)
        pNotifier->onAreaChanged.connect(this, &Series::onAreaChanged);
    OnStudyClosed.connect(this, &Series::onStudyClosed);

    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(notebook, 1, wxALL | wxEXPAND);
    SetSizer(sizer);

    OnStudyLoaded.connect(this, &Series::onStudyLoaded);
}

Series::~Series()
{
    destroyBoundEvents();
}

void Series::onAreaChanged(Data::Area* area)
{
    pLastArea = CurrentStudyIsValid() ? area : nullptr;
}

void Series::onStudyClosed()
{
    pLastArea = nullptr;
    if (pNotifier)
        pNotifier->onAreaChanged(nullptr);
}

void Series::onStudyLoaded()
{
    if (pPageFatal)
        pPageFatal->select();
}

} // namespace Hydro
} // namespace Window
} // namespace Antares
