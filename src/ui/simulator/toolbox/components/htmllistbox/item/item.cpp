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

#include "item.h"
#include <wx/colour.h>
#include <wx/regex.h>
#include <yuni/core/math.h>

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Item
{
IItem::IItem() : pHighlighted(false), pVisible(true)
{
}

IItem::~IItem()
{
}

bool HTMLCodeHighlightString(wxString& s, const wxString& toHighlight)
{
    if (!toHighlight.empty())
    {
        wxRegEx regex(wxString(wxT("(")) << toHighlight << wxT(")"), wxRE_ICASE | wxRE_DEFAULT);
        if (regex.Matches(s))
        {
            const wxString& rep = regex.GetMatch(s, 1);
            regex.ReplaceFirst(
              &s, (wxString(wxT("<font color=red><u><b>")) << rep << wxT("</b></u></font>")));
            return true;
        }
    }
    return false;
}

wxColour ColorDarker(int r, int g, int b, int level)
{
    return wxColour(Yuni::Math::MinMax<int>(r - level, 0, 255),
                    Yuni::Math::MinMax<int>(g - level, 0, 255),
                    Yuni::Math::MinMax<int>(b - level, 0, 255));
}

wxColour ColorDarker(const wxColour& c, int level)
{
    return wxColour(Yuni::Math::MinMax<int>((int)c.Red() - level, 0, 255),
                    Yuni::Math::MinMax<int>((int)c.Green() - level, 0, 255),
                    Yuni::Math::MinMax<int>((int)c.Blue() - level, 0, 255));
}

} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
