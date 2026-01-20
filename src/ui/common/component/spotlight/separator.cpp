// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "spotlight.h"
#include <wx/dcbuffer.h>
#include <wx/settings.h>
#include "listbox-panel.h"

using namespace Yuni;

#define NODE_DRAW_COLOR_VARIATION_LIGHT 30
#define NODE_DRAW_COLOR_VARIATION_DARK 15

namespace Antares::Component
{
//! The default background color
static wxColour gColorBackgroundDefault;

Spotlight::Separator::Separator()
{
    // This item should not be taken into consideration in the result set
    pCountedAsResult = false;

    if (!gColorBackgroundDefault.IsOk())
    {
        wxColour c = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
        gColorBackgroundDefault.Set((int)Math::MinMax<int>(c.Red() - 35, 0, 255),
                                    (int)Math::MinMax<int>(c.Green() - 35, 0, 255),
                                    (int)Math::MinMax<int>(c.Blue() - 35, 0, 255));
    }
}

Spotlight::Separator::~Separator()
{
    // destructor
}

void Spotlight::Separator::draw(wxDC& dc,
                                uint itemHeight,
                                wxRect& bounds,
                                bool /*selected*/,
                                const SearchToken::VectorPtr&) const
{
    enum
    {
        tagHeight = 18,
        border = 6
    };

    dc.SetPen(wxPen(gColorBackgroundDefault, 1, wxPENSTYLE_SOLID));
    dc.DrawLine(bounds.x + border,
                bounds.y + itemHeight / 2,
                bounds.width + bounds.x - border * 3,
                bounds.y + itemHeight / 2);

    bounds.y += itemHeight;
}

} // namespace Antares::Component
