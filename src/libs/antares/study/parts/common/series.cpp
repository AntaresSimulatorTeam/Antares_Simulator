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

#include <yuni/yuni.h>
#include <yuni/io/file.h>
#include <yuni/io/directory.h>
#include "../../study.h"
#include "series.h"

using namespace Yuni;

#define SEP IO::Separator


namespace Antares::Data
{
bool DataSeriesCommon::forceReload(bool reload) const
{
    return timeSeries.forceReload(reload);
}

void DataSeriesCommon::markAsModified() const
{
    timeSeries.markAsModified();
}

uint64_t DataSeriesCommon::memoryUsage() const
{
    return timeSeries.memoryUsage();
}

double DataSeriesCommon::getValue(unsigned int hour, unsigned int year) const
{
    if (timeSeries.width == 1)
        return timeSeries[0][hour];
    else
    {
        const unsigned int tsIndex = timeseriesNumbers[0][year];
        return timeSeries[tsIndex][hour];
    }
}
} // namespace Antares::Data

