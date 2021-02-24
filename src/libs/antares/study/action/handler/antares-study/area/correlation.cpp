/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "correlation.h"
#include "correlation-post.h"

namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Area
{
Correlation::Correlation(Data::TimeSeries ts, const AnyString& areaname) :
 pType(ts), pOriginalAreaName(areaname)
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

} // namespace Area
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares
