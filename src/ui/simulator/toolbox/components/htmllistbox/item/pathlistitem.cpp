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

#include "pathlistitem.h"
#include <wx/colour.h>

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
                               Yuni::uint64 size)
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
