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

#include "renewable-cluster.h"
#include <wx/colour.h>
#include "../../../resources.h"
#include <yuni/core/math.h>

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Item
{
wxString RenewableCluster::pIconFileEnabled;
wxString RenewableCluster::pIconFileDisabled;
wxString RenewableCluster::pIconFileThermal;

RenewableCluster::RenewableCluster(Antares::Data::RenewableCluster* a) : pRenewableCluster(a)
{
    preloadImages();
}

RenewableCluster::RenewableCluster(Antares::Data::RenewableCluster* a, const wxString& additional) :
 pRenewableCluster(a), pText(additional)
{
    preloadImages();
}

RenewableCluster::~RenewableCluster()
{
}

void RenewableCluster::preloadImages()
{
    if (pIconFileEnabled.empty())
    {
        String location;

        Resources::FindFile(location, "images/16x16/light_green.png");
        pIconFileEnabled = wxStringFromUTF8(location);

        Resources::FindFile(location, "images/16x16/light_orange.png");
        pIconFileDisabled = wxStringFromUTF8(location);

        Resources::FindFile(location, "images/16x16/thermal.png");
        pIconFileThermal = wxStringFromUTF8(location);
    }
}

bool RenewableCluster::HtmlContent(wxString& out,
                                 Data::RenewableCluster* rn,
                                 const wxString& searchString)
{
    bool highlight = false;

    if (rn->enabled)
    {
        out << wxT("<td width=30 align=center><img src=\"") << pIconFileEnabled << wxT("\"></td>");
    }
    else
    {
        out << wxT("<td width=30 align=center><img src=\"") << pIconFileDisabled << wxT("\"></td>");
    }

    out << wxT("<td width=20 align=center><img src=\"") << pIconFileThermal << wxT("\"></td>");

    out << wxT("<td width=8></td><td nowrap><font size=\"-1\"");
    wxString name = wxStringFromUTF8(rn->name());
    if (searchString.empty() || (highlight = HTMLCodeHighlightString(name, searchString)))
        out << wxT(">") << name << wxT("</font>");
    else
        out << wxT(" color=\"#999999\">") << name << wxT("</font>");
    // Post
    out << wxT("</td>");
    return highlight;
}

wxString RenewableCluster::htmlContent(const wxString& searchString)
{
    // gp : to be adapted
    if (pRenewableCluster)
    {
        wxString d;
        d << wxT("<table border=0 cellpadding=0 cellspacing=0 width=\"100%\"><tr>");
        pHighlighted = HtmlContent(d, pRenewableCluster, searchString);
        d << wxT("<td nowrap align=right><font size=\"-2\">") /* << pRenewableCluster->unitCount */
          << wxT("<font color=\"#5555BB\"> u </font>") << wxT("<font color=\"#5555BB\">* </font>")
          << pRenewableCluster->nominalCapacity
          << wxT(" <font color=\"#5555BB\">MW =</font></font></td>")
          << wxT("<td width=64 nowrap align=right><font size=\"-2\">")
          // << Math::Round(pRenewableCluster->nominalCapacity * pRenewableCluster->unitCount, 2)
          << wxT(" <font color=\"#5555BB\">MW</font></font></td>")
          << wxT("<td width=90 nowrap align=right><font size=\"-2\">")
          // << Math::Round(pRenewableCluster->marketBidCost, 3)
          << wxT(" <font color=\"#DD3311\">\u20AC/MWh</font></font></td>")
          << wxT("<td width=5></td>");
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
