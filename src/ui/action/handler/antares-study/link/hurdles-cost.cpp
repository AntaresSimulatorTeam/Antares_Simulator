// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "hurdles-cost.h"
#include "antares/utils/utils.h"
#include <antares/study/area/constants.h>
#include <antares/utils/utils.h>

namespace Antares::Action::AntaresStudy::Link
{
HurdlesCost::HurdlesCost(const AnyString& fromarea, const AnyString& toarea) :
 pOriginalFromAreaName(fromarea), pOriginalToAreaName(toarea)
{
    pInfos.caption << "Hurdles-Cost";
}

HurdlesCost::~HurdlesCost()
{
}

bool HurdlesCost::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The hurdles-cost will be copied";
        break;
    default:
        pInfos.state = stNothingToDo;
        break;
    }

    return true;
}

bool HurdlesCost::performWL(Context& ctx)
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
            source->parameters.forceReload(true);
            ctx.link->parameters.forceReload(true);

            ctx.link->parameters.pasteToColumn(
              (uint)Data::fhlHurdlesCostDirect,
              source->parameters.entry[Data::fhlHurdlesCostDirect]);
            ctx.link->parameters.pasteToColumn(
              (uint)Data::fhlHurdlesCostIndirect,
              source->parameters.entry[Data::fhlHurdlesCostIndirect]);
            ctx.link->useHurdlesCost = source->useHurdlesCost;

            ctx.link->assetType = source->assetType;
            return true;
        }
    }
    return false;
}

} // namespace Antares::Action::AntaresStudy::Link
