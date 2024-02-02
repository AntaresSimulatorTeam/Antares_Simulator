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
