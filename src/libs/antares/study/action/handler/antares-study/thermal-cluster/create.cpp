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

#include "create.h"

using namespace Yuni;

namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace ThermalCluster
{
Create::Create(const AnyString& areaname, const AnyString& clustername) :
 pOriginalAreaName(areaname), pOriginalPlantName(clustername)
{
    pInfos.behavior = bhOverwrite;
}

Create::~Create()
{
}

bool Create::prepareWL(Context& ctx)
{
    pInfos.message.clear();

    // Assign the cluster name if not already done
    // The variable pTargetPlantName may be slighty different from the original
    // name (changed from the interface for example)
    if (pTargetPlantName.empty())
        pTargetPlantName = pOriginalPlantName;
    pFuturPlantName = pTargetPlantName;

    // The name can not be empty
    if (pFuturPlantName.empty())
    {
        pInfos.caption = "Unknown thermal cluster";
        pInfos.message << "The name must not be empty";
        pInfos.state = stError;
        return false;
    }

    // Computing the futur ID of the area
    Data::ClusterName id;
    const Data::ThermalCluster* clusterFound = nullptr;

    // finding the final area
    const Data::AreaName& areaMapping = ctx.areaNameMapping[pOriginalAreaName];
    const Data::Area* area = ctx.study.areas.findFromName(areaMapping);
    bool forceAreaCreate = ctx.areaForceCreate[pOriginalAreaName];
    bool forcePlantCreate = ctx.clusterForceCreate[pOriginalAreaName];

    // the suffix
    const String& suffix = ctx.property["cluster.name.suffix"];

    if (not suffix.empty() || pInfos.behavior == bhMerge)
    {
        if (suffix == "<auto>" || pInfos.behavior == bhMerge)
        {
            TransformNameIntoID(pFuturPlantName, id);
            clusterFound = area ? area->thermal.list.find(id) : nullptr;
            if (clusterFound)
            {
                Data::AreaName::Size sepPos = id.find_last_of('-');
                if (sepPos != YString::npos)
                {
                    Data::AreaName suffixChain(id, sepPos + 1);
                    int suffixNumber = suffixChain.to<int>();
                    if (suffixNumber > 0)
                    {
                        Data::AreaName suffixLess(pFuturPlantName, 0, sepPos);
                        pFuturPlantName = suffixLess;
                    }
                }
                uint indx = 1;
                do
                {
                    ++indx;
                    pFuturPlantName.clear() << pTargetPlantName << "-" << indx;
                    id.clear();
                    TransformNameIntoID(pFuturPlantName, id);
                    clusterFound = area->thermal.list.find(id);
                } while (clusterFound);
            }
        }
        else
        {
            pFuturPlantName += suffix;
            TransformNameIntoID(pFuturPlantName, id);
            clusterFound = area ? area->thermal.list.find(id) : nullptr;
        }
    }
    else
    {
        TransformNameIntoID(pFuturPlantName, id);
        clusterFound = area ? area->thermal.list.find(id) : nullptr;
    }

    pInfos.caption.clear() << "Plant " << pFuturPlantName;
    // Default state
    pInfos.state = stReady;

    if (clusterFound && !forceAreaCreate && !forcePlantCreate)
    {
        // The area
        switch (pInfos.behavior)
        {
        case bhMerge:
            pInfos.message << "The cluster '" << pFuturPlantName
                           << "' already exist and will remain untouched";
            break;
        case bhOverwrite:
            pInfos.message << "The cluster '" << pFuturPlantName << "' will be reset";
            pInfos.state = stReady;
            break;
        default:
            break;
        }
    }
    else
    {
        // The area does not exist it will be created then
        pInfos.message << "The cluster '" << pFuturPlantName << "' will be created";
        pInfos.state = stReady;
    }
    return true;
}

bool Create::performWL(Context& ctx)
{
    ctx.originalPlant = nullptr;
    ctx.cluster = nullptr;
    if (ctx.area)
    {
        Data::ClusterName id;

        // source cluster
        Data::Area* source = ctx.extStudy->areas.findFromName(pOriginalAreaName);
        if (!source)
            return false;
        TransformNameIntoID(pOriginalPlantName, id);
        ctx.originalPlant = source->thermal.list.find(id);
        if (!ctx.originalPlant)
            return false;

        // new cluster
        id.clear();
        TransformNameIntoID(pFuturPlantName, id);

        ctx.cluster = ctx.area->thermal.list.find(id);
        if (!ctx.cluster)
        {
            ctx.cluster = new Data::ThermalCluster(ctx.area);
            ctx.cluster->name(pFuturPlantName);
            ctx.cluster->reset();
            (ctx.area)->thermal.list.add(ctx.cluster);
            (ctx.area)->thermal.list.rebuildIndex();
            (ctx.area)->thermal.prepareAreaWideIndexes();
        }
        else
        {
            if (pInfos.behavior == bhOverwrite)
                (ctx.cluster)->reset();
        }
        return (ctx.area != NULL);
    }
    return true;
}

} // namespace ThermalCluster
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares
