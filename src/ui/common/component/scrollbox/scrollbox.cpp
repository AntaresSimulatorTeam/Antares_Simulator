// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "scrollbox.h"
#include <wx/sizer.h>

namespace Antares::Component
{
IMPLEMENT_DYNAMIC_CLASS(Scrollbox, wxScrolledWindow)

BEGIN_EVENT_TABLE(Scrollbox, wxScrolledWindow)
END_EVENT_TABLE()

Scrollbox::Scrollbox()
{
}

Scrollbox::Scrollbox(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size):
    wxScrolledWindow(parent, id, pos, size, wxTAB_TRAVERSAL)
{
    SetAutoLayout(true);
}

Scrollbox::~Scrollbox()
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

} // namespace Antares::Component
