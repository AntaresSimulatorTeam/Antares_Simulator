// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "copper-plate.h"
#include <antares/utils/utils.h>

namespace Antares::Action::AntaresStudy::Link
{
CopperPlate::CopperPlate(const AnyString& fromarea, const AnyString& toarea) :
 pOriginalFromAreaName(fromarea), pOriginalToAreaName(toarea)
{
    pInfos.caption << "Transmission capacities";
}

CopperPlate::~CopperPlate()
{
}

bool CopperPlate::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The transmission capacities mode will be copied";
        break;
    default:
        pInfos.state = stNothingToDo;
        break;
    }

    return true;
}

bool CopperPlate::performWL(Context& ctx)
{
    if (ctx.link && ctx.extStudy)
    {
        Data::AreaName idFrom;
        Data::AreaName idTo;
        TransformNameIntoID(pOriginalFromAreaName, idFrom);
        TransformNameIntoID(pOriginalToAreaName, idTo);

        Data::AreaLink* source;
        if (pOriginalFromAreaName < pOriginalToAreaName)
            source = ctx.extStudy->areas.findLink(idFrom, idTo);
        else
            source = ctx.extStudy->areas.findLink(idTo, idFrom);

        if (source && source != ctx.link)
        {
            ctx.link->transmissionCapacities = source->transmissionCapacities;
            return true;
        }
    }
    return false;
}

} // namespace Antares::Action::AntaresStudy::Link



