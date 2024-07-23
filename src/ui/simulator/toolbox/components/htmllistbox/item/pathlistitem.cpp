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
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "pathlistitem.h"
#include "wx-wrapper.h"
#include <wx/wx.h>

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Item
{
PathListItem::PathListItem(const Clob& a, const PathList::FileInfo& opts) :
 pText(wxStringFromUTF8(a)), pSize(opts.size), pIsFolder(0 != (opts.options & pathListOptFolder))
{
}

bool PathListItem::HtmlContent(wxString& out,
                               wxString name,
                               const wxString& searchString,
                               bool isFolder,
                               uint64_t size)
{
    wxColour c(220, 220, 250);
    if (isFolder)
        c.Set(250, 200, 200);
    bool highlight = false;
    out << wxT("<td width=5></td><td width=16 bgcolor=\"") << c.GetAsString(wxC2S_HTML_SYNTAX)
        << wxT("\" align=center><font size=\"-3\" color=\"")
        << ColorDarker(c.Red(), c.Green(), c.Blue()).GetAsString(wxC2S_HTML_SYNTAX) << wxT("\">");
    if (isFolder)
        out << wxT("D");
    else
        out << wxT("F");
    out << wxT("</font></td><td width=8></td><td nowrap><font size=\"-1\"");
    if (searchString.empty() || (highlight = HTMLCodeHighlightString(name, searchString)))
        out << wxT(">") << name << wxT("</font>");
    else
        out << wxT(" color=\"#999999\">") << name << wxT("</font>");

    out << wxT("</td><td align=\"right\" width=\"30\" NOWRAP>");
    // Size
    if (size > 1)
    {
        if (size < 1024)
            out << wxT("<font size=\"-2\" color=\"#A9B2DB\">") << size << wxT(" bytes</font>");
        else
        {
            if (size < 1024 * 1024)
                out << wxT("<font size=\"-2\" color=\"#A9B2DB\">") << (size / 1024)
                    << wxT(" Kb</font>");
            else
                out << wxT("<font size=\"-2\" color=\"#A9B2DB\">") << (size / 1024 / 1024)
                    << wxT(" Mb</font>");
        }
    }
    // Post
    out << wxT("</td>");
    return highlight;
}

wxString PathListItem::htmlContent(const wxString& searchString)
{
    if (!pText.empty())
    {
        wxString d;
        d << wxT("<table border=0 cellpadding=0 cellspacing=0 width=\"98%\"><tr>");
        pHighlighted = HtmlContent(d, pText, searchString, pIsFolder, pSize);
        // Post
        d << wxT("</tr></table>");
        return d;
    }
    pHighlighted = false;
    return wxString();
}

} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
