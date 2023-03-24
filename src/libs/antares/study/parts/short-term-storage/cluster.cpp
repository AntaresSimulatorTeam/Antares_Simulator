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

#define SEP Yuni::IO::Separator

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
            logs.warning() << "Loading clusters: `" << section.name << "`/`"
                << property->key << "`: The property is unknown and ignored";
        }
    }

    if (properties.name.empty())
        return false;

    logs.notice() << "before yuni";
    Yuni::CString<50, false> tmp;
    TransformNameIntoID(properties.name, tmp);
    id = tmp.c_str();
    logs.notice() << "after yuni";

    return true;
}

bool STStorageCluster::fillDefaultSeries()
{
    if (series.maxInjection.empty())
        series.maxInjection.resize(VECTOR_SERIES_SIZE, properties.injectionCapacity);
    if (series.maxWithdrawal.empty())
        series.maxWithdrawal.resize(VECTOR_SERIES_SIZE, properties.withdrawalCapacity);
    if (series.inflows.empty())
        series.inflows.resize(VECTOR_SERIES_SIZE, 0);
    if (series.lowerRuleCurve.empty())
        series.lowerRuleCurve.resize(VECTOR_SERIES_SIZE, 0);
    if (series.upperRuleCurve.empty())
        series.upperRuleCurve.resize(VECTOR_SERIES_SIZE, 1);

    return true;
}

bool STStorageCluster::validate()
{
    return properties.validate() && series.validate();
}

bool STStorageCluster::loadSeries(const std::string& folder)
{
    bool ret = series.loadFromFolder(folder + SEP);
    ret = fillDefaultSeries() && ret; //fill series if no file series
    return ret;
}

} // namespace Antares::Data::ShortTermStorage
