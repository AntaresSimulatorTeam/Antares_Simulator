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

#include "area.h"
#include "wx-wrapper.h"
#include <wx/colour.h>

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Item
{
Area::Area(Data::Area* a) : pArea(a)
{
}

Area::Area(Data::Area* a, const wxString& additional) : pArea(a), pText(additional)
{
}

Area::~Area()
{
}

bool Area::HtmlContent(wxString& out, Data::Area* area, const wxString& searchString)
{
    bool highlight = false;
    out << wxT("<td width=5></td><td width=16 bgcolor=\"")
        << wxColour(area->ui->color[0], area->ui->color[1], area->ui->color[2])
             .GetAsString(wxC2S_HTML_SYNTAX)
        << wxT("\" align=center><font size=\"-3\" color=\"")
        << ColorDarker(area->ui->color[0], area->ui->color[1], area->ui->color[2])
             .GetAsString(wxC2S_HTML_SYNTAX)
        << wxT("\">Ar</font></td><td width=8></td><td nowrap><font size=\"-1\"");
    wxString name = wxStringFromUTF8(area->name);
    if (searchString.empty() || (highlight = HTMLCodeHighlightString(name, searchString)))
        out << wxT(">") << name << wxT("</font>");
    else
        out << wxT(" color=\"#999999\">") << name << wxT("</font>");
    // Post
    out << wxT("</td>");
    return highlight;
}

wxString Area::htmlContent(const wxString& searchString)
{
    if (pArea)
    {
        wxString d;
        d << wxT("<table border=0 cellpadding=0 cellspacing=0 width=\"100%\"><tr>");
        pHighlighted = HtmlContent(d, pArea, searchString);
        // Post
        d << pText << wxT("</tr></table>");
        return d;
    }
    pHighlighted = false;
    return wxEmptyString;
}

} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
