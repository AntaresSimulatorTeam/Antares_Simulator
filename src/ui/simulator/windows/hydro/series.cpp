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
    
    datagridNum = 3;
    AllocateComponents(notebook);

    com[0]->renderer(new Component::Datagrid::Renderer::TimeSeriesHydroFatal(com[0], notifier));
    pPageFatal = notebook->add(com[0], wxT("Run-of-the-river (ROR)"));

    com[1]->renderer(new Component::Datagrid::Renderer::TimeSeriesHydroMod(com[1], notifier));    
    notebook->add(com[1], wxT("Hydro Storage"));

    com[2]->renderer(new Component::Datagrid::Renderer::TimeSeriesHydroMinGen(com[2], notifier));
    notebook->add(com[2], wxT("Minimum Generation"));

    // Connection to the notifier
    if (pNotifier)
        pNotifier->onAreaChanged.connect(this, &Series::onAreaChanged);
    OnStudyClosed.connect(this, &Series::onStudyClosed);

    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(notebook, 1, wxALL | wxEXPAND);
    SetSizer(sizer);

    OnStudyLoaded.connect(this, &Series::onStudyLoaded);
}

void Series::AllocateComponents(Component::Notebook* notebook)
{
    com = new Component::Datagrid::Component*[datagridNum];
    for (uint i = 0; i < datagridNum; i++)
        com[i] = new Component::Datagrid::Component(notebook);
}

void Series::DeallocateComponents()
{
    for (uint i = 0; i < datagridNum; i++)
        delete (com + i);
    delete[] com;
    com = nullptr;
}

Series::~Series()
{
    destroyBoundEvents();
    DeallocateComponents();
}

void Series::onAreaChanged(Data::Area* area)
{
    pLastArea = Data::Study::Current::Valid() ? area : nullptr;
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
