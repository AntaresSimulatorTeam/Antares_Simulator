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
#include "../../../../../utils.h"
#include "nodal-optimization.h"
#include "reserves.h"
#include "misc-gen.h"
#include "timeseries.h"
#include "ts-generator.h"
#include "position.h"
#include "color.h"
#include "ts-node.h"
#include "correlation.h"
#include "allocation-hydro.h"
#include "filtering.h"
#include "../thermal-cluster/create.h"
#include "../thermal-cluster/root-node.h"
#include "../thermal-cluster/common.h"

using namespace Yuni;

namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Area
{
Create::Create(const AnyString& areaname) : pOriginalAreaName(areaname)
{
    pInfos.behavior = bhOverwrite;
}

Create::Create(const AnyString& areaname, const AnyString& targetname) :
 pOriginalAreaName(areaname), pTargetAreaName(targetname)
{
    pInfos.behavior = bhOverwrite;
}

Create::~Create()
{
}

void Create::prepareSkipWL(Context& ctx)
{
    if (pTargetAreaName.empty())
        pTargetAreaName = pOriginalAreaName;

    Data::AreaName originalID;
    TransformNameIntoID(pOriginalAreaName, originalID);
    // area mapping
    ctx.areaNameMapping[pOriginalAreaName] = pTargetAreaName;
    ctx.areaLowerNameMapping[originalID] = pFuturAreaName;
    ctx.areaForceCreate[pOriginalAreaName] = false;
    logs.debug() << "  copy/paste: mapping: " << pOriginalAreaName << " -> " << pTargetAreaName;
    ctx.originalPlant = nullptr;
}

bool Create::prepareWL(Context& ctx)
{
    pInfos.message.clear();
    ctx.originalPlant = nullptr;

    bool explicitTarget = true;
    // Assign the area name if not already done
    // The variable pTargetAreaName may be slighty different from the original
    // name (changed from the interface for example)
    if (pTargetAreaName.empty())
    {
        pTargetAreaName = pOriginalAreaName;
        explicitTarget = false;
    }
    pFuturAreaName = pTargetAreaName;

    ctx.areaForceCreate[pOriginalAreaName] = false;

    // The name can not be empty
    if (pFuturAreaName.empty())
    {
        pInfos.caption = "Unknown area";
        pInfos.message << "The name must not be empty";
        pInfos.state = stError;
        // area mapping
        ctx.areaNameMapping[pOriginalAreaName].clear();

        Data::AreaName originalID;
        TransformNameIntoID(pOriginalAreaName, originalID);
        ctx.areaLowerNameMapping[originalID].clear();
        return false;
    }

    // Computing the futur ID of the area
    Data::AreaName id;
    const Antares::Data::Area* areaFound = nullptr;
    // the suffix
    const String& suffix = ctx.property["area.name.suffix"];

    if (not suffix.empty() || !explicitTarget)
    {
        if (suffix == "<auto>" || !explicitTarget)
        {
            TransformNameIntoID(pFuturAreaName, id);
            areaFound = ctx.study.areas.find(id);
            if (areaFound)
            {
                Data::AreaName::Size sepPos = id.find_last_of('-');
                if (sepPos != YString::npos)
                {
                    Data::AreaName suffixChain(id, sepPos + 1);
                    int suffixNumber = suffixChain.to<int>();
                    if (suffixNumber > 0)
                    {
                        Data::AreaName suffixLess(pFuturAreaName, 0, sepPos);
                        pTargetAreaName = suffixLess;
                    }
                }
                unsigned int indx = 1;
                do
                {
                    ++indx;
                    pFuturAreaName.clear() << pTargetAreaName << "-" << indx;
                    id.clear();
                    TransformNameIntoID(pFuturAreaName, id);
                    areaFound = ctx.study.areas.find(id);
                } while (areaFound);
            }
        }
        else
        {
            pFuturAreaName += suffix;
            TransformNameIntoID(pFuturAreaName, id);
            areaFound = ctx.study.areas.find(id);
        }
    }
    else
    {
        TransformNameIntoID(pFuturAreaName, id);
        areaFound = ctx.study.areas.find(id);
    }

    // area mapping
    ctx.areaNameMapping[pOriginalAreaName] = pFuturAreaName;

    Data::AreaName originalID;
    TransformNameIntoID(pOriginalAreaName, originalID);
    ctx.areaLowerNameMapping[originalID] = pFuturAreaName;
    logs.debug() << "  copy/paste: mapping: " << pOriginalAreaName << " -> " << pFuturAreaName;

    pInfos.caption.clear() << "Area " << pFuturAreaName;
    // Default state
    pInfos.state = stReady;

    if (areaFound)
    {
        // The area
        switch (pInfos.behavior)
        {
        case bhMerge:
            pInfos.message << "The area '" << pFuturAreaName
                           << "' already exists and will remain untouched";
            break;
        case bhOverwrite:
            pInfos.message << "The area '" << pFuturAreaName << "' will be reset";
            ctx.areaForceCreate[pOriginalAreaName] = true;
            break;
        default:
            break;
        }
    }
    else
    {
        // The area does not exist it will be created then
        pInfos.message << "The area '" << pFuturAreaName << "' will be created";
        ctx.areaForceCreate[pOriginalAreaName] = true;
    }
    return true;
}

bool Create::performWL(Context& ctx)
{
    // assert
    assert(not pFuturAreaName.empty() && "An area name must not be empty");

    ctx.message.clear() << "Updating the area " << pFuturAreaName;
    ctx.updateMessageUI(ctx.message);

    ctx.originalPlant = nullptr;

    Data::AreaName id;
    TransformNameIntoID(pFuturAreaName, id);
    ctx.area = ctx.study.areas.find(id);

    // The area
    if (not ctx.area)
    {
        // create the area
        ctx.area = ctx.study.areaAdd(pFuturAreaName);
        logs.debug() << "[study-action] The area " << pFuturAreaName << " has been created";
    }
    else
    {
        logs.debug() << "[study-action] The area " << pFuturAreaName << " has been found";
        if (pInfos.behavior == bhOverwrite)
        {
            logs.debug() << "[study-action] The area " << pFuturAreaName << " has been reset";
            //(ctx.area)->resetToDefaultValues();
        }
    }
    ctx.autoselectAreas.push_back(ctx.area);
    return (ctx.area != NULL);
}

void Create::createActionsForAStandardAreaCopy(Context& ctx, bool copyPosition)
{
    // UI
    // Position
    auto* p = new Position(pOriginalAreaName);
    if (!copyPosition)
        p->behavior(bhSkip);
    *this += p;
    // Color
    auto* c = new Color(pOriginalAreaName);
    if (copyPosition)
        c->behavior(bhOverwrite);

    *this += c;

    // Nodal optimization
    *this += new NodalOptimization(pOriginalAreaName);
    // Reserves
    *this += new Reserves(pOriginalAreaName);
    // Misc Gen.
    *this += new MiscGen(pOriginalAreaName);
    // Filtering
    *this += new Filtering(pOriginalAreaName);

    NodeTimeseries* tsNode;
    DataTSGenerator* prepro;
    // Load
    *this += (tsNode = new NodeTimeseries(Data::timeSeriesLoad));
    *tsNode += new DataTimeseries(Data::timeSeriesLoad, pOriginalAreaName);
    prepro = new DataTSGenerator(Data::timeSeriesLoad, pOriginalAreaName);
    *prepro += new Correlation(Data::timeSeriesLoad, pOriginalAreaName);
    *tsNode += prepro;

    // Solar
    *this += (tsNode = new NodeTimeseries(Data::timeSeriesSolar));
    *tsNode += new DataTimeseries(Data::timeSeriesSolar, pOriginalAreaName);
    prepro = new DataTSGenerator(Data::timeSeriesSolar, pOriginalAreaName);
    *prepro += new Correlation(Data::timeSeriesSolar, pOriginalAreaName);
    *tsNode += prepro;

    // Wind
    *this += (tsNode = new NodeTimeseries(Data::timeSeriesWind));
    *tsNode += new DataTimeseries(Data::timeSeriesWind, pOriginalAreaName);
    prepro = new DataTSGenerator(Data::timeSeriesWind, pOriginalAreaName);
    *prepro += new Correlation(Data::timeSeriesWind, pOriginalAreaName);
    *tsNode += prepro;

    // Hydro
    *this += (tsNode = new NodeTimeseries(Data::timeSeriesHydro));
    *tsNode += new DataTimeseries(Data::timeSeriesHydro, pOriginalAreaName);
    prepro = new DataTSGenerator(Data::timeSeriesHydro, pOriginalAreaName);
    *prepro += new Correlation(Data::timeSeriesHydro, pOriginalAreaName);
    *tsNode += prepro;
    *tsNode += new Action::AntaresStudy::Area::AllocationHydro(pOriginalAreaName);

    // Thermal
    auto* area = ctx.extStudy->areas.findFromName(pOriginalAreaName);
    if (area)
    {
        typedef Antares::Action::AntaresStudy::ThermalCluster::RootNode RootNodePlant;
        auto* root = new RootNodePlant(pOriginalAreaName);

        // browsing each thermal cluster
        auto end = area->thermal.list.end();
        for (auto i = area->thermal.list.begin(); i != end; ++i)
        {
            *root += StandardActionsToCopyThermalCluster(pOriginalAreaName, i->second->name());
        }
        *this += root;
    }
}

IAction* Create::StandardActionsToCopyThermalCluster(const Data::AreaName& area,
                                                     const Data::ThermalClusterName& name)
{
    typedef Antares::Action::AntaresStudy::ThermalCluster::Create NodePlant;
    typedef Antares::Action::AntaresStudy::ThermalCluster::CommonData NodePlantCommonData;

    auto* cluster = new NodePlant(area, name);
    // TS
    *cluster += new NodePlantCommonData();
    *cluster += new DataTimeseries(Data::timeSeriesThermal, area);
    *cluster += new DataTSGenerator(Data::timeSeriesThermal, area);
    return cluster;
}

} // namespace Area
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares
