// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "unload-data.h"
#include <antares/utils/utils.h>

namespace Antares::Action::AntaresStudy::Link
{
UnloadData::UnloadData(const AnyString& fromarea, const AnyString& toarea):
    pOriginalFromAreaName(fromarea),
    pOriginalToAreaName(toarea)
{
    pInfos.caption << "Unload Data from memory";
}

UnloadData::~UnloadData()
{
}

bool UnloadData::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The UnloadData will be copied";
        break;
    default:
        pInfos.state = stNothingToDo;
        break;
    }

    return true;
}

bool UnloadData::performWL(Context& ctx)
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
            source->parameters.unloadFromMemory();
            source->directCapacities.unloadFromMemory();
            source->indirectCapacities.unloadFromMemory();
            return true;
        }
    }
    return false;
}

} // namespace Antares::Action::AntaresStudy::Link
