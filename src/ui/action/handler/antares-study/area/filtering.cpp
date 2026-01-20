// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "filtering.h"

using namespace Yuni;

namespace Antares::Action::AntaresStudy::Area
{
Filtering::Filtering(const AnyString& areaname):
    pOriginalAreaName(areaname)
{
    pInfos.caption << "Filtering";
}

Filtering::~Filtering()
{
}

bool Filtering::prepareWL(Context& /*ctx*/)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The filtering properties of the area will be copied";
        break;
    default:
        pInfos.state = stDisabled;
        break;
    }
    return true;
}

bool Filtering::performWL(Context& ctx)
{
    if (ctx.area && ctx.extStudy && ctx.area->ui)
    {
        Data::Area* source = ctx.extStudy->areas.findFromName(pOriginalAreaName);
        if (source && source->ui)
        {
            ctx.area->filterSynthesis = source->filterSynthesis;
            ctx.area->filterYearByYear = source->filterYearByYear;
            return true;
        }
    }
    return false;
}

} // namespace Antares::Action::AntaresStudy::Area
