// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "comments.h"
#include <antares/utils/utils.h>

namespace Antares::Action::AntaresStudy::Link
{
bool Comments::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The comments will be copied";
        break;
    default:
        pInfos.state = stNothingToDo;
        break;
    }

    return true;
}

bool Comments::performWL(Context& ctx)
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
            ctx.link->comments = source->comments;
            ctx.link->displayComments = source->displayComments;
            return true;
        }
    }
    return false;
}

} // namespace Antares::Action::AntaresStudy::Link
