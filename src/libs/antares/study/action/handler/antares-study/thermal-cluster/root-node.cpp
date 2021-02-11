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

#include "root-node.h"

namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace ThermalCluster
{
RootNode::RootNode(const AnyString& areaname) : pOriginalAreaName(areaname)
{
    pInfos.caption << "Thermal clusters";
    pInfos.behavior = bhOverwrite;
}

RootNode::~RootNode()
{
}

void RootNode::prepareSkipWL(Context& ctx)
{
    ctx.clusterForceCreate[pOriginalAreaName] = false;
}

bool RootNode::prepareWL(Context& ctx)
{
    pInfos.message.clear();
    pInfos.state = stReady;

    bool forceAreaCreate = ctx.areaForceCreate[pOriginalAreaName];
    ctx.clusterForceCreate[pOriginalAreaName] = (forceAreaCreate || pInfos.behavior == bhOverwrite);

    switch (pInfos.behavior)
    {
    case bhMerge:
    {
        if (!forceAreaCreate)
        {
            pInfos.message << "The thermal cluster list will remain untouched";
            pInfos.state = stNothingToDo;
        }
        break;
    }
    case bhOverwrite:
        pInfos.message << "The thermal cluster list will be reset";
        break;
    default:
        pInfos.state = stDisabled;
        break;
    }

    return true;
}

bool RootNode::performWL(Context& ctx)
{
    if (ctx.area)
    {
        // bool forcePlantCreate = ctx.clusterForceCreate[pOriginalAreaName];
        if (pInfos.behavior == bhOverwrite)
            ctx.area->thermal.list.clear();
        return true;
    }
    return false;
}

} // namespace ThermalCluster
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares
