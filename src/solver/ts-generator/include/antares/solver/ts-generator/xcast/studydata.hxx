// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
