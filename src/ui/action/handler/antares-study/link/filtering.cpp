// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "filtering.h"
#include <antares/utils/utils.h>

using namespace Yuni;

namespace Antares::Action::AntaresStudy::Link
{
Filtering::Filtering(const AnyString& fromarea, const AnyString& toarea):
    pOriginalFromAreaName(fromarea),
    pOriginalToAreaName(toarea)
{
    pInfos.caption << "Filtering";
}

Filtering::~Filtering()
{
}

bool Filtering::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The filtering properties of the link will be copied";
        break;
    default:
        pInfos.state = stNothingToDo;
        break;
    }

    return true;
}

bool Filtering::performWL(Context& ctx)
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
            ctx.link->filterSynthesis = source->filterSynthesis;
            ctx.link->filterYearByYear = source->filterYearByYear;
            return true;
        }
    }
    return false;
}

} // namespace Antares::Action::AntaresStudy::Link
