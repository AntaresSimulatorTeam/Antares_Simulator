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

#include "bindingconstraint.h"
#include "../../../resources.h"

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Item
{
wxString BindingConstraint::pIconFileEnabled;
wxString BindingConstraint::pIconFileDisabled;

BindingConstraint::BindingConstraint(Data::BindingConstraint* a) : pBindingConstraint(a)
{
    preloadImages();
}

BindingConstraint::BindingConstraint(Data::BindingConstraint* a, const wxString& additional) :
 pBindingConstraint(a), pText(additional)
{
    preloadImages();
}

BindingConstraint::~BindingConstraint()
{
}

void BindingConstraint::preloadImages()
{
    if (pIconFileEnabled.empty())
    {
        Yuni::String location;

        Resources::FindFile(location, "images/16x16/light_green.png");
        pIconFileEnabled = wxStringFromUTF8(location);

        Resources::FindFile(location, "images/16x16/light_orange.png");
        pIconFileDisabled = wxStringFromUTF8(location);
    }
}

bool BindingConstraint::HtmlContent(wxString& out,
                                    Data::BindingConstraint* bc,
                                    const wxString& searchString)
{
    bool highlight = false;

    if (bc->enabled() && bc->linkCount() > 0)
    {
        out << wxT("<td width=45 align=center><img src=\"") << pIconFileEnabled << wxT("\"></td>");
    }
    else
    {
        out << wxT("<td width=45 align=center><img src=\"") << pIconFileDisabled << wxT("\"></td>");
    }

    out << wxT("<td width=22 bgcolor=\"#7485BE\" align=center><font size=\"-3\" "
               "color=\"#444E6F\">Bc</font></td>");

    out << wxT("<td width=8></td><td nowrap><font size=\"-1\"");
    wxString name = wxStringFromUTF8(bc->name());
    if (searchString.empty() || (highlight = HTMLCodeHighlightString(name, searchString)))
        out << wxT("><b>") << name << wxT("</b></font>");
    else
        out << wxT(" color=\"#999999\">") << name << wxT("</font>");

    out << wxT("<font size=\"-2\" color=\"#AAAAAA\"><br><i>  (")
        << wxStringFromUTF8(Data::BindingConstraint::OperatorToShortCString(bc->operatorType()))
        << wxT(", ") << wxStringFromUTF8(Data::BindingConstraint::TypeToCString(bc->type()))
        << wxT(")</i></font>");

    // Post
    Yuni::String s;
    s.reserve(512);
    bc->buildHTMLFormula(s);
    out << wxT("<font size=\"-2\"> ") << wxStringFromUTF8(s) << wxT("</font>");
    if (not bc->comments().empty())
        out << wxT("<font size=\"-2\" color=\"#363F59\"><br>") << wxStringFromUTF8(bc->comments())
            << wxT("</font>");
    out << wxT("</td>");
    return highlight;
}

wxString BindingConstraint::htmlContent(const wxString& searchString)
{
    if (pBindingConstraint)
    {
        wxString d;
        d << wxT("<table border=0 cellpadding=0 cellspacing=0 width=\"100%\"><tr>");
        pHighlighted = HtmlContent(d, pBindingConstraint, searchString);
        // Post
        d << pText << wxT("</tr></table>");
        return d;
    }
    pHighlighted = false;
    return wxString();
}

} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
