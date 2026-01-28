// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "correlation.h"
#include "correlation-post.h"

namespace Antares::Action::AntaresStudy::Area
{
Correlation::Correlation(Data::TimeSeriesType ts, const AnyString& areaname):
    pType(ts),
    pOriginalAreaName(areaname)
{
    switch (ts)
    {
    case Data::timeSeriesLoad:
        pInfos.caption << "Load : Correlation";
        break;
    case Data::timeSeriesSolar:
        pInfos.caption << "Solar : Correlation";
        break;
    case Data::timeSeriesWind:
        pInfos.caption << "Wind : Correlation";
        break;
    case Data::timeSeriesHydro:
        pInfos.caption << "Hydro : Correlation";
        break;
    case Data::timeSeriesThermal:
        pInfos.caption << "Thermal : Correlation";
        break;
    default:
        break;
    }
}

Correlation::~Correlation()
{
}

bool Correlation::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The correlation coefficients will be copied";
        break;
    default:
        pInfos.state = stNothingToDo;
        break;
    }

    return true;
}

bool Correlation::performWL(Context&)
{
    // Nothing to do
    return true;
}

void Correlation::createPostActionsWL(const IAction::Ptr& node)
{
    // all actions on correlation must be delayed
    // (we must have all areas, thus it must be done at the end of the merge)
    *node += new CorrelationPost(this, pType, pOriginalAreaName);
}

} // namespace Antares::Action::AntaresStudy::Area
