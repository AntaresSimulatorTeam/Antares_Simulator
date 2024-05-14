/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_SOLVER_TS_GENERATOR_XCAST_STUDY_DATA_HXX__
#define __ANTARES_SOLVER_TS_GENERATOR_XCAST_STUDY_DATA_HXX__

#include <yuni/yuni.h>

#include <antares/logs/logs.h>
#include <antares/study/study.h>
#include <antares/study/xcast/xcast.h>

namespace Antares::TSGenerator::XCast
{
template<class PredicateT>
void StudyData::loadFromStudy(Data::Study& study,
                              const Data::Correlation& correlation,
                              PredicateT& predicate)
{
    // clear our list of areas
    localareas.clear();

    // We will rebuild it using the predicate
    for (auto i = study.areas.begin(); i != study.areas.end(); ++i)
    {
        auto& area = *(i->second);
        if (predicate.accept(area))
        {
            // XCast will have to use this area
            logs.info() << "  Added the area '" << area.name << "'";
            localareas.push_back(&area);
        }
    }
    reloadDataFromAreaList(correlation);
}

} // namespace Antares::TSGenerator::XCast

#endif // __ANTARES_SOLVER_TS_GENERATOR_XCAST_STUDY_DATA_HXX__
