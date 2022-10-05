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

#include "study.h"

using namespace Yuni;


namespace Antares
{
namespace Data
{

bool Study::initializeInternalData(const StudyLoadOptions& options)
{

    if (usedByTheSolver and not options.prepareOutput)
    {
        parameters.noOutput = true;
        parameters.yearByYear = false;
        parameters.timeSeriesToArchive = 0;
        parameters.storeTimeseriesNumbers = false;
        parameters.synthesis = false;
    }

    if (options.loadOnlyNeeded and !parameters.timeSeriesToGenerate)
        // Nothing to refresh
        parameters.timeSeriesToRefresh = 0;
    

    // We can not run the simulation if the study folder is not in the latest
    // version and that we would like to re-importe the generated timeseries
    if (usedByTheSolver)
    {
        // We have time-series to import
        if (parameters.timeSeriesToImport and (uint) header.version != (uint)versionLatest)
        {
            logs.error() << "Stochastic TS stored in input : study must be upgraded to "
                         << Data::VersionToCStr((Data::Version)Data::versionLatest);
            gotFatalError = true;
            // it is useless to continue at this point
            return false;
        }
    }

    // calendar update
    if (usedByTheSolver)
        calendar.reset(parameters, /*force leapyear:*/ false);
    else
        calendar.reset(parameters);

    calendarOutput.reset(parameters);

    // In case hydro hot start is enabled, check all conditions are met.
    // (has to be called after areas load and calendar building)
    if (usedByTheSolver && !checkHydroHotStart())
        return false;

    // Reducing memory footprint
    reduceMemoryUsage();
    return true;

}

class SetHandlerAreas
{
public:
    SetHandlerAreas(Study& study) : pStudy(study)
    {
    }

    void clear(Study::SingleSetOfAreas& set)
    {
        set.clear();
    }

    uint size(Study::SingleSetOfAreas& set)
    {
        return (uint)set.size();
    }

    bool add(Study::SingleSetOfAreas& set, const String& value)
    {
        Area* area = AreaListLFind(&pStudy.areas, value.c_str());
        if (area)
        {
            set.insert(area);
            return true;
        }
        return false;
    }

    bool add(Study::SingleSetOfAreas& set, const Study::SingleSetOfAreas& otherSet)
    {
        if (!otherSet.empty())
        {
            auto end = otherSet.end();
            for (auto i = otherSet.begin(); i != end; ++i)
                set.insert(*i);
        }
        return true;
    }

    bool remove(Study::SingleSetOfAreas& set, const String& value)
    {
        Area* area = AreaListLFind(&pStudy.areas, value.c_str());
        if (area)
        {
            set.erase(area);
            return true;
        }
        return false;
    }

    bool remove(Study::SingleSetOfAreas& set, const Study::SingleSetOfAreas& otherSet)
    {
        if (!otherSet.empty())
        {
            auto end = otherSet.end();
            for (auto i = otherSet.begin(); i != end; ++i)
            {
                set.erase(*i);
            }
        }
        return true;
    }

    bool applyFilter(Study::SingleSetOfAreas& set, const String& value)
    {
        if (value == "add-all")
        {
            auto end = pStudy.areas.end();
            for (auto i = pStudy.areas.begin(); i != end; ++i)
                set.insert(i->second);
            return true;
        }

        if (value == "remove-all")
        {
            set.clear();
            return true;
        }
        return false;
    }

private:
    Study& pStudy;

}; // class SetHandlerAreas 

void Study::initializeSetsData()
{
    // Apply the rules
    SetHandlerAreas handler(*this);
    setsOfAreas.rebuildAllFromRules(handler);
    // Write the results into the logs
    setsOfAreas.dumpToLogs(logs);
}

} // namespace Data
} // namespace Antares
