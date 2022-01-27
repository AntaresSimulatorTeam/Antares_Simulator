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
    ClusterItem(a),
    pThermalCluster(a)
{
    preloadImages();
}

ThermalClusterItem::ThermalClusterItem(Antares::Data::ThermalCluster* a, const wxString& additional) :
    ClusterItem(a, additional),
    pThermalCluster(a)
{
    preloadImages();
}

ThermalClusterItem::~ThermalClusterItem()
{
}

wxString ThermalClusterItem::getClusterIconFilePath()
{
    return getIconFilePath("images/16x16/thermal.png");
}

wxString ThermalClusterItem::htmlContentTail()
{
    wxString tail_to_return;
    tail_to_return
        << Math::Round(pThermalCluster->marketBidCost, 3)
        << wxT(" <font color=\"#DD3311\">\u20AC/MWh</font></font></td>")
        << wxT("<td width=5></td>");
    return tail_to_return;
}

Antares::Data::ThermalCluster* ThermalClusterItem::thermalAggregate() const
{
    return pThermalCluster;
}

} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
