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
#include <yuni/io/directory.h>
#include "../../study.h"
#include "../../memory-usage.h"
#include "series.h"

using namespace Yuni;

#define SEP IO::Separator

namespace Antares
{
namespace Data
{
int DataSeriesSaveToFolder(const DataSeriesCommon* t,
                           const RenewableCluster* ag,
                           const AnyString& folder)
{
    if (t && ag and not folder.empty())
    {
        Clob buffer;

        buffer.clear() << folder << SEP << ag->parentArea->id << SEP << ag->id();
        if (IO::Directory::Create(buffer))
        {
            int ret = 1;
            buffer.clear() << folder << SEP << ag->parentArea->id << SEP << ag->id() << SEP
                           << "series.txt";
            ret = t->series.saveToCSVFile(buffer, 0) && ret;

            return ret;
        }
        return 0;
    }
    return 1;
}

int DataSeriesLoadFromFolder(Study& s,
                             DataSeriesCommon* t,
                             RenewableCluster* ag,
                             const AnyString& folder)
{
    if (t and ag and not folder.empty())
    {
        auto& buffer = s.bufferLoadingTS;

        int ret = 1;
        buffer.clear() << folder << SEP << ag->parentArea->id << SEP << ag->id() << SEP << "series."
                       << s.inputExtension;
        ret = t->series.loadFromCSVFile(buffer, 1, HOURS_PER_YEAR, &s.dataBuffer) && ret;

        if (s.usedByTheSolver && s.parameters.derated)
            t->series.averageTimeseries();

        t->timeseriesNumbers.clear();

        return ret;
    }
    return 1;
}
} // namespace Data
} // namespace Antares
