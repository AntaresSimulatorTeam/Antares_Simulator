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

#include "allocation.h"
#include "../../toolbox/components/notebook/notebook.h"
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/components/datagrid/renderer/area/hydro/allocation.h"
#include <wx/sizer.h>

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace Hydro
{
Allocation::Allocation(wxWindow* parent) : Component::Panel(parent)
{
    auto* com = new Component::Datagrid::Component(this);
    com->renderer(new Component::Datagrid::Renderer::HydroAllocation());

    auto* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(com, 1, wxALL | wxEXPAND);
    SetSizer(sizer);
}

Allocation::~Allocation()
{
    // To avoid corrupt vtable in some rare cases / misuses
    // (when children try to access to this object for example),
    // we should destroy all children as soon as possible.
    auto* sizer = GetSizer();
    if (sizer)
        sizer->Clear(true);
}

} // namespace Hydro
} // namespace Window
} // namespace Antares
