// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "root-node.h"

namespace Antares::Action::AntaresStudy::ThermalCluster
{
RootNode::RootNode(const AnyString& areaname):
    pOriginalAreaName(areaname)
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
        {
            ctx.area->thermal.list.clearAll();
        }
        return true;
    }
    return false;
}

} // namespace Antares::Action::AntaresStudy::ThermalCluster
