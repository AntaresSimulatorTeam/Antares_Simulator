// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "item.h"
#include <wx/colour.h>
#include <wx/regex.h>
#include <yuni/core/math.h>

namespace Antares::Component::HTMLListbox::Item
{
IItem::IItem():
    pHighlighted(false),
    pVisible(true)
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
            regex.ReplaceFirst(&s,
                               (wxString(wxT("<font color=red><u><b>"))
                                << rep << wxT("</b></u></font>")));
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

} // namespace Antares::Component::HTMLListbox::Item
