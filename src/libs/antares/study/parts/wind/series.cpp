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

#include <yuni/yuni.h>
#include <yuni/io/file.h>
#include "series.h"
#include "../../study.h"
#include "../../memory-usage.h"

using namespace Yuni;

#define SEP IO::Separator

namespace Antares
{
namespace Data
{
int DataSeriesWindLoadFromFolder(Study& s,
                                 DataSeriesWind* d,
                                 const AreaName& areaID,
                                 const char folder[])
{
    if (!d)
        return 1;
    String& buffer = s.bufferLoadingTS;

    int ret = 1;
    buffer.clear() << folder << SEP << "wind_" << areaID << '.' << s.inputExtension;
    ret = d->series.loadFromCSVFile(buffer, 1, HOURS_PER_YEAR, &s.dataBuffer) && ret;

    if (s.usedByTheSolver && s.parameters.derated)
        d->series.averageTimeseries();

    d->timeseriesNumbers.clear();
    d->series.flush();

    return ret;
}

int DataSeriesWindSaveToFolder(DataSeriesWind* d, const AreaName& areaID, const char folder[])
{
    if (!d)
        return 1;

    Clob buffer;
    int ret = 1;
    buffer.clear() << folder << SEP << "wind_" << areaID << ".txt";
    ret = d->series.saveToCSVFile(buffer, 0) && ret;

    return ret;
}

bool DataSeriesWind::invalidate(bool reload) const
{
    return series.invalidate(reload);
}

void DataSeriesWind::markAsModified() const
{
    series.markAsModified();
}

void DataSeriesWind::estimateMemoryUsage(StudyMemoryUsage& u) const
{
    u.requiredMemoryForInput += sizeof(DataSeriesWind);
    timeseriesNumbers.estimateMemoryUsage(u, true, 1, u.years);
    series.estimateMemoryUsage(u,
                               0 != (timeSeriesWind & u.study.parameters.timeSeriesToGenerate),
                               u.study.parameters.nbTimeSeriesWind,
                               HOURS_PER_YEAR);
}

} // namespace Data
} // namespace Antares
