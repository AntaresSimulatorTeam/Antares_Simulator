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

#include "renewable-cluster-item.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Item
{

RenewableClusterItem::RenewableClusterItem(Antares::Data::RenewableCluster* a) :
    ClusterItem(a),
    pRenewableCluster(a)
{
    preloadImages();
}

RenewableClusterItem::RenewableClusterItem(Antares::Data::RenewableCluster* a, const wxString& additional) :
    ClusterItem(a, additional),
    pRenewableCluster(a)
{
    preloadImages();
}

RenewableClusterItem::~RenewableClusterItem()
{
}

wxString RenewableClusterItem::getClusterIconFilePath()
{
    // gp : This icon file (link.png) is given here as an example, for test purposes.
    // gp : It has to be chnaged when renewable icon is ready 
    return getIconFilePath("images/16x16/link.png");
}

wxString RenewableClusterItem::htmlContentTail()
{
    // Return nothing 
    return wxString();
}

Antares::Data::RenewableCluster* RenewableClusterItem::renewableAggregate() const
{
    return pRenewableCluster;
}

} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
