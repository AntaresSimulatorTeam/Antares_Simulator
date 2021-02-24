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
#include <stdio.h>
#include "series.h"
#include "../../study.h"
#include "../../memory-usage.h"

using namespace Yuni;

#define SEP IO::Separator

namespace Antares
{
namespace Data
{
int DataSeriesLoadLoadFromFolder(Study& study,
                                 DataSeriesLoad* s,
                                 const AreaName& areaID,
                                 const char* folder)
{
    /* Assert */
    assert(s);
    assert(folder);
    assert('\0' != *folder);

    auto& buffer = study.buffer;

    int ret = 1;
    /* Load the matrix */
    buffer.clear() << folder << SEP << "load_" << areaID << '.' << study.inputExtension;
    ret = s->series.loadFromCSVFile(buffer, 1, HOURS_PER_YEAR, &study.dataBuffer) && ret;

    if (study.usedByTheSolver && study.parameters.derated)
        s->series.averageTimeseries();

    s->timeseriesNumbers.clear();
    s->series.flush();

    return ret;
}

int DataSeriesLoadSaveToFolder(DataSeriesLoad* s, const AreaName& areaID, const char folder[])
{
    if (!s)
        return 1;

    /* Assert */
    assert(folder);
    assert('\0' != *folder);

    if (IO::Directory::Create(folder))
    {
        int res = 1;

        Clob buffer;
        buffer.clear() << folder << SEP << "load_" << areaID << ".txt";
        res = s->series.saveToCSVFile(buffer, /*decimal*/ 0) && res;

        return res;
    }
    return 0;
}

bool DataSeriesLoad::invalidate(bool reload) const
{
    return series.invalidate(reload);
}

void DataSeriesLoad::markAsModified() const
{
    return series.markAsModified();
}

void DataSeriesLoad::estimateMemoryUsage(StudyMemoryUsage& u) const
{
    u.requiredMemoryForInput += sizeof(DataSeriesLoad);
    timeseriesNumbers.estimateMemoryUsage(u, true, 1, u.years);
    series.estimateMemoryUsage(u,
                               0 != (timeSeriesLoad & u.study.parameters.timeSeriesToGenerate),
                               u.study.parameters.nbTimeSeriesLoad,
                               HOURS_PER_YEAR);
}

} // namespace Data
} // namespace Antares
