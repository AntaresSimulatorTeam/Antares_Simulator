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

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Item
{

RenewableCluster::RenewableCluster(Antares::Data::RenewableCluster* a) :
    pRenewableCluster(a),
    pCluster(a)
{
    preloadImages();
}

RenewableCluster::RenewableCluster(Antares::Data::RenewableCluster* a, const wxString& additional) :
    ClusterItem(additional),
    pRenewableCluster(a),
    pCluster(a)
{
    preloadImages();
}

RenewableCluster::~RenewableCluster()
{
}

wxString RenewableCluster::getClusterIconFilePath()
{
    // gp : This icon file (link.png) is given here as an example, for test purposes.
    // gp : It has to be chnaged when renewable icon is ready 
    return getIconFilePath("images/16x16/link.png");
}

wxString RenewableCluster::htmlContent(const wxString& searchString)
{
    if (pCluster)
    {
        wxString d;
        d << wxT("<table border=0 cellpadding=0 cellspacing=0 width=\"100%\"><tr>");
        pHighlighted = HtmlContent(d, pCluster, searchString);
        d << wxT("<td nowrap align=right><font size=\"-2\">") << pCluster->unitCount
            << wxT("<font color=\"#5555BB\"> u </font>") << wxT("<font color=\"#5555BB\">* </font>")
            << pCluster->nominalCapacity
            << wxT(" <font color=\"#5555BB\">MW =</font></font></td>")
            << wxT("<td width=64 nowrap align=right><font size=\"-2\">")
            << Math::Round(pCluster->nominalCapacity * pCluster->unitCount, 2)
            << wxT(" <font color=\"#5555BB\">MW</font></font></td>")
            << wxT("<td width=90 nowrap align=right><font size=\"-2\">");
        // Post
        d << pText << wxT("</tr></table>");
        return d;
    }
    pHighlighted = false;
    return wxString();
}

Antares::Data::RenewableCluster* RenewableCluster::renewableAggregate() const
{
    return pRenewableCluster;
}

} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
