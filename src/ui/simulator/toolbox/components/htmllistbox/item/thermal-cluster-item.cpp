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

#include "thermal-cluster-item.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Item
{
ThermalClusterItem::ThermalClusterItem(Antares::Data::ThermalCluster* a) :
 ClusterItem(a), pThermalCluster(a)
{
    preloadImages();
    preloadAdditionalIcons();
}

ThermalClusterItem::ThermalClusterItem(Antares::Data::ThermalCluster* a,
                                       const wxString& additional) :
 ClusterItem(a, additional), pThermalCluster(a)
{
    preloadImages();
    preloadAdditionalIcons();
}

ThermalClusterItem::~ThermalClusterItem()
{
}

void ThermalClusterItem::preloadAdditionalIcons()
{
    pIconFileRefresh = getIconFilePath("images/16x16/G.png");
    pIconFileNoRefresh = getIconFilePath("images/16x16/NG.png");
}

wxString ThermalClusterItem::htmlContentTail()
{
    wxString tail_to_return;
    tail_to_return << Math::Round(pThermalCluster->marketBidCost, 3)
                   << wxT(" <font color=\"#DD3311\">\u20AC/MWh</font></font></td>")
                   << wxT("<td width=5></td>");
    return tail_to_return;
}

void ThermalClusterItem::addAdditionalIcons(wxString& out) const
{
    using Behavior = Data::LocalTSGenerationBehavior;
    if (pThermalCluster->tsGenBehavior == Behavior::forceGen)
    {
        out << wxT("<td width=20 align=center><img src=\"") << pIconFileRefresh << wxT("\"></td>");
    }

    if (pThermalCluster->tsGenBehavior == Behavior::forceNoGen)
    {
        out << wxT("<td width=20 align=center><img src=\"") << pIconFileNoRefresh
            << wxT("\"></td>");
    }
}

Antares::Data::ThermalCluster* ThermalClusterItem::thermalAggregate() const
{
    return pThermalCluster;
}

} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
