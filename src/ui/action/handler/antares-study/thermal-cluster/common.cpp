// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "common.h"

using namespace Yuni;

namespace Antares::Action::AntaresStudy::ThermalCluster
{
CommonData::CommonData()
{
    pInfos.caption = "Common data";
}

CommonData::~CommonData()
{
}

bool CommonData::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The common data will be copied";
        break;
    default:
        pInfos.state = stDisabled;
        break;
    }

    return true;
}

bool CommonData::performWL(Context& ctx)
{
    if (ctx.area && ctx.extStudy)
    {
        if (ctx.cluster && ctx.originalPlant && ctx.cluster != ctx.originalPlant)
        {
            // copy the data
            ctx.cluster->copyFrom(*(ctx.originalPlant));
            return true;
        }
    }
    return false;
}

} // namespace Antares::Action::AntaresStudy::ThermalCluster
