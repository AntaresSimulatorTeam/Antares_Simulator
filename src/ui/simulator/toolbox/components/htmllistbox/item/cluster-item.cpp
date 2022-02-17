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

#include "cluster-item.h"
#include "../../../resources.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Item
{
ClusterItem::ClusterItem(Antares::Data::Cluster* a) : pCluster(a), pText(wxString())
{
}

ClusterItem::ClusterItem(Antares::Data::Cluster* a, const wxString& additional) :
 pCluster(a), pText(additional)
{
}

wxString ClusterItem::getIconFilePath(const AnyString& filename)
{
    String location;
    Resources::FindFile(location, filename);
    return wxStringFromUTF8(location);
}

void ClusterItem::preloadImages()
{
    if (pIconFileEnabled.empty())
    {
        pIconFileEnabled = getIconFilePath("images/16x16/light_green.png");
        pIconFileDisabled = getIconFilePath("images/16x16/light_orange.png");
    }
}

void ClusterItem::addAdditionalIcons(wxString& /* out */) const
{
    // No additional icons should be added by default
}

bool ClusterItem::HtmlContent(wxString& out, Data::Cluster* cluster, const wxString& searchString)
{
    bool highlight = false;

    if (cluster->enabled)
    {
        out << wxT("<td width=20 align=center><img src=\"") << pIconFileEnabled << wxT("\"></td>");
    }
    else
    {
        out << wxT("<td width=20 align=center><img src=\"") << pIconFileDisabled << wxT("\"></td>");
    }

    // Specific to thermal clusters
    addAdditionalIcons(out);

    out << wxT("<td width=8></td><td nowrap><font size=\"-1\"");

    wxString name = wxStringFromUTF8(cluster->name());
    if (searchString.empty() || (highlight = HTMLCodeHighlightString(name, searchString)))
        out << wxT(">") << name << wxT("</font>");
    else
        out << wxT(" color=\"#999999\">") << name << wxT("</font>");
    // Post
    out << wxT("</td>");
    return highlight;
}

wxString ClusterItem::htmlContent(const wxString& searchString)
{
    if (pCluster)
    {
        wxString d;
        d << wxT("<table border=0 cellpadding=0 cellspacing=0 width=\"100%\"><tr>");
        pHighlighted = HtmlContent(d, pCluster, searchString);
        d << wxT("<td nowrap align=right><font size=\"-2\">") << pCluster->unitCount
          << wxT("<font color=\"#5555BB\"> u </font>") << wxT("<font color=\"#5555BB\">* </font>")
          << pCluster->nominalCapacity << wxT(" <font color=\"#5555BB\">MW =</font></font></td>")
          << wxT("<td width=64 nowrap align=right><font size=\"-2\">")
          << Math::Round(pCluster->nominalCapacity * pCluster->unitCount, 2)
          << wxT(" <font color=\"#5555BB\">MW</font></font></td>")
          << wxT("<td width=90 nowrap align=right><font size=\"-2\">") << htmlContentTail();
        // Post
        d << pText << wxT("</tr></table>");
        return d;
    }
    pHighlighted = false;
    return wxString();
}

ClusterItem::~ClusterItem()
{
}

} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
