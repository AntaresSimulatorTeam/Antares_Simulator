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

#include "antares/study/parts/short-term-storage/ManagedTimeSeries.h"

namespace Antares::Data::ShortTermStorage
{
ManagedTimeSeries::ManagedTimeSeries():
    series(tsNumbers)
{
}

ManagedTimeSeries::ManagedTimeSeries(const ManagedTimeSeries& other):
    tsNumbers(other.tsNumbers),
    series(tsNumbers)
{
    series.timeSeries = other.series.timeSeries;
}

ManagedTimeSeries::ManagedTimeSeries(ManagedTimeSeries&& other) noexcept:
    tsNumbers(std::move(other.tsNumbers)),
    series(tsNumbers)
{
    series.timeSeries = std::move(other.series.timeSeries);
}

ManagedTimeSeries& ManagedTimeSeries::operator=(const ManagedTimeSeries& other)
{
    if (this != &other)
    {
        tsNumbers = other.tsNumbers;
        series.timeSeries = other.series.timeSeries;
    }
    return *this;
}

ManagedTimeSeries& ManagedTimeSeries::operator=(ManagedTimeSeries&& other) noexcept
{
    if (this != &other)
    {
        tsNumbers = std::move(other.tsNumbers);
        series.timeSeries = std::move(other.series.timeSeries);
    }
    return *this;
}
} // namespace Antares::Data::ShortTermStorage
