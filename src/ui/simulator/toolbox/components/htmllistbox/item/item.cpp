/*
** Copyright 2007-2018 RTE
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
