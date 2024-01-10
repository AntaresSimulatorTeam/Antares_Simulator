/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "spotlight.h"
#include <wx/dcbuffer.h>
#include <wx/settings.h>
#include "listbox-panel.h"

using namespace Yuni;

#define NODE_DRAW_COLOR_VARIATION_LIGHT 30
#define NODE_DRAW_COLOR_VARIATION_DARK 15

namespace Antares
{
namespace Component
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

} // namespace Component
} // namespace Antares
