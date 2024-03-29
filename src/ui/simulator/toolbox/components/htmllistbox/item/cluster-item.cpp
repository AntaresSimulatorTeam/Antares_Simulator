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

#include "cluster-item.h"
#include "../../../resources.h"
#include "wx-wrapper.h"

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
