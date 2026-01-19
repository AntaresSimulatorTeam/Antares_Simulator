// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "ntc.h"
#include <antares/utils/utils.h>

namespace Antares::Action::AntaresStudy::Link
{
NTC::NTC(const AnyString& fromarea, const AnyString& toarea):
    pOriginalFromAreaName(fromarea),
    pOriginalToAreaName(toarea)
{
    pInfos.caption << "NTC";
}

NTC::~NTC()
{
}

bool NTC::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The NTC will be copied";
        break;
    default:
        pInfos.state = stNothingToDo;
        break;
    }

    return true;
}

bool NTC::performWL(Context& ctx)
{
    if (ctx.link && ctx.extStudy)
    {
        Data::AreaName idFrom;
        Data::AreaName idTo;
        TransformNameIntoID(pOriginalFromAreaName, idFrom);
        TransformNameIntoID(pOriginalToAreaName, idTo);

        Data::AreaLink* source;
        if (pOriginalFromAreaName < pOriginalToAreaName)
        {
            source = ctx.extStudy->areas.findLink(idFrom, idTo);
        }
        else
        {
            source = ctx.extStudy->areas.findLink(idTo, idFrom);
        }

        if (source && source != ctx.link)
        {
            // Direct capacities
            source->directCapacities.forceReload(true);
            ctx.link->directCapacities.forceReload(true);
            ctx.link->directCapacities.timeSeries
              .pasteToColumn(0, source->directCapacities.timeSeries.entry[0]);

            // Indirect capacities
            source->indirectCapacities.forceReload(true);
            ctx.link->indirectCapacities.forceReload(true);
            ctx.link->indirectCapacities.timeSeries
              .pasteToColumn(0, source->indirectCapacities.timeSeries.entry[0]);
            return true;
        }
        else
        {
            if (!source)
            {
                logs.error() << "Impossible to find the link " << idFrom << " - " << idTo;
            }
        }
    }
    return false;
}

} // namespace Antares::Action::AntaresStudy::Link
