// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "thermal-cluster-item.h"

using namespace Yuni;

namespace Antares::Component::HTMLListbox::Item
{
ThermalClusterItem::ThermalClusterItem(Antares::Data::ThermalCluster* a):
    ClusterItem(a),
    pThermalCluster(a)
{
    preloadImages();
    preloadAdditionalIcons();
}

ThermalClusterItem::ThermalClusterItem(Antares::Data::ThermalCluster* a,
                                       const wxString& additional):
    ClusterItem(a, additional),
    pThermalCluster(a)
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

} // namespace Antares::Component::HTMLListbox::Item
