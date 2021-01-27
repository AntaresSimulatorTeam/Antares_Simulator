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
#ifndef __ANTARES_SOLVER_TS_GENERATOR_XCAST_STUDY_DATA_HXX__
#define __ANTARES_SOLVER_TS_GENERATOR_XCAST_STUDY_DATA_HXX__

#include <yuni/yuni.h>
#include <antares/study/xcast/xcast.h>
#include <antares/logs.h>

namespace Antares
{
namespace Solver
{
namespace TSGenerator
{
namespace XCast
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
        else
        {
            // resize and set the values for the matrix
            predicate.matrix(area).reset(1, HOURS_PER_YEAR);
        }
    }
    reloadDataFromAreaList(correlation);
}

} // namespace XCast
} // namespace TSGenerator
} // namespace Solver
} // namespace Antares

#endif // __ANTARES_SOLVER_TS_GENERATOR_XCAST_STUDY_DATA_HXX__
