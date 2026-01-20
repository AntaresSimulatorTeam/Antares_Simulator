// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "allocation.h"
#include "../../toolbox/components/notebook/notebook.h"
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/components/datagrid/renderer/area/hydro/allocation.h"
#include <wx/sizer.h>

using namespace Yuni;

namespace Antares::Window::Hydro
{
Allocation::Allocation(wxWindow* parent):
    Component::Panel(parent)
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
    {
        sizer->Clear(true);
    }
}

} // namespace Antares::Window::Hydro
