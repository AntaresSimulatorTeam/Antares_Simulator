/*
** Copyright 2007-2023 RTE
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
#include <antares/utils.h>
#include <antares/logs.h>
#include <yuni/core/string.h>
#include <yuni/io/file.h>

#include "cluster.h"

namespace Antares::Data::ShortTermStorage
{

bool STStorageCluster::loadFromSection(const IniFile::Section& section)
{
    if (!section.firstProperty)
        return false;

    for (auto* property = section.firstProperty; property; property = property->next)
    {
        if (property->key.empty())
        {
            logs.warning() << "Loading clusters: `" << section.name << "`: Invalid key/value";
            continue;
        }
        if (!properties.loadKey(property))
        {
            logs.warning() << "Loading clusters: `" << section.name << "`/`" << property->key
                           << "`: The property is unknown and ignored";
        }
    }

    if (properties.name.empty())
    {
        logs.warning() << "Missing name for short-term storage " << section.name;
        return false;
    }

    Yuni::CString<50, false> tmp;
    TransformNameIntoID(properties.name, tmp);
    id = tmp.to<std::string>();

    return true;
}

bool STStorageCluster::loadSeries(const std::string& folder) const
{
    bool ret = series->loadFromFolder(folder);
    series->fillDefaultSeriesIfEmpty(); // fill series if no file series
    return ret;
}

bool STStorageCluster::validate(bool simplexIsWeek)
{
    logs.debug() << "Validating properties and series for st storage: " << id;
    return properties.validate(simplexIsWeek) && series->validate();
}

bool STStorageCluster::validateWeeklyTimeseries(unsigned int firstHourOfTheWeek) const
{
    logs.debug() << "Validating cycles in series for st storage: " << id;

    return series->validateCycleForWeek(firstHourOfTheWeek,
                                        properties.initialLevel,
                                        properties.storagecycle.value()) &&
           series->validateInflowsForWeek(firstHourOfTheWeek,
                                          properties.storagecycle.value(),
                                          properties.injectionCapacity.value(),
                                          properties.withdrawalCapacity.value());
}

void STStorageCluster::computeInitLevelBoundsAtCycles(unsigned int firstHourOfTheWeek,
                                                      bool isSimplexWeekly)
{
    // Caution : 
    //      we compute the bounds for initial levels only when initial level is not given by user.
    //      In that case, this initial level is computed by optimization, and that's the reason
    //      why we need bounds for it at each start hour of a cycle.
    
    if (properties.initialLevel.has_value())
        return;

    // TO DO : 
    //      Normally, the choice and the storage of the level bound calculator should be
    //      done in the cluster's constructor, so that we don't have to do it 
    //      every weeks as here and now. At least, we should do it once. 
    if (isSimplexWeekly)
    {
        levelBoundsCalculator_ = std::make_shared<LevelBoundsForWeeks>(
            firstHourOfTheWeek,
            firstHourOfTheWeek + Constants::nbHoursInAWeek,
            properties.storagecycle.value(),
            series->lowerRuleCurve,
            series->upperRuleCurve);
    }
    else
    {
        levelBoundsCalculator_ = std::make_shared<LevelBoundsForDays>(
            firstHourOfTheWeek,
            firstHourOfTheWeek + Constants::nbHoursInAWeek,
            properties.storagecycle.value(),
            series->lowerRuleCurve,
            series->upperRuleCurve);
    }

    levelBoundsCalculator_->addBounds(initLevelBounds_);
}

Bounds STStorageCluster::getInitLevelBoundsAtHour(unsigned int hour)
{
    return initLevelBounds_.at(hour);
}

} // namespace Antares::Data::ShortTermStorage
