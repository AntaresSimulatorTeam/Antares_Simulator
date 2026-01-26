// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "impedances.h"
#include <antares/study/area/constants.h>
#include <antares/utils/utils.h>

namespace Antares::Action::AntaresStudy::Link
{
Impedances::Impedances(const AnyString& fromarea, const AnyString& toarea):
    pOriginalFromAreaName(fromarea),
    pOriginalToAreaName(toarea)
{
    pInfos.caption << "Impedances";
}

Impedances::~Impedances()
{
}

bool Impedances::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The Impedances will be copied";
        break;
    default:
        pInfos.state = stNothingToDo;
        break;
    }

    return true;
}

bool Impedances::performWL(Context& ctx)
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
            source->parameters.forceReload(true);
            ctx.link->parameters.forceReload(true);
            ctx.link->parameters.pasteToColumn((uint)Data::fhlImpedances,
                                               source->parameters.entry[Data::fhlImpedances]);
            return true;
        }
    }
    return false;
}

} // namespace Antares::Action::AntaresStudy::Link
