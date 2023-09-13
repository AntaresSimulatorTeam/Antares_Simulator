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

#include <antares/writer/i_writer.h>
#include <antares/study/study.h>
#include <antares/logs/logs.h>

#include "store-timeseries-numbers.h"

using namespace Yuni;

#define SEP IO::Separator

namespace Antares::Data
{
namespace // anonymous
{
struct TSNumbersPredicate
{
    uint32 operator()(uint32 value) const
    {
        return value + 1;
    }
};
} // anonymous namespace

static void genericStoreTimeseriesNumbers(Solver::IResultWriter::Ptr writer,
                                          const Matrix<Yuni::uint32>& timeseriesNumbers,
                                          const String& id,
                                          const String& directory)
{
    TSNumbersPredicate predicate;
    Clob path;
    path << "ts-numbers" << SEP << directory << SEP << id << ".txt";

    std::string buffer;
    timeseriesNumbers.saveToBuffer(buffer,
                                   0,         // precision
                                   true,      // print_dimensions
                                   predicate, // predicate
                                   true);     // save even if all coeffs are zero

    writer->addEntryFromBuffer(path.c_str(), buffer);
}

void storeTimeseriesNumbersForLoad(Solver::IResultWriter::Ptr writer, const Area& area)
{
    genericStoreTimeseriesNumbers(writer, area.load.series->timeseriesNumbers, area.id, "load");
}

void storeTimeseriesNumbersForSolar(Solver::IResultWriter::Ptr writer, const Area& area)
{
    genericStoreTimeseriesNumbers(writer, area.solar.series->timeseriesNumbers, area.id, "solar");
}

void storeTimeseriesNumbersForHydro(Solver::IResultWriter::Ptr writer, const Area& area)
{
    genericStoreTimeseriesNumbers(writer, area.hydro.series->timeseriesNumbers, area.id, "hydro");
}


void storeTimeseriesNumbersForHydroPowerCredits(Solver::IResultWriter::Ptr writer, const Area& area)
{
    genericStoreTimeseriesNumbers(writer, area.hydro.series->timeseriesNumbersPowerCredits, area.id, "power-credits");
}

void storeTimeseriesNumbersForWind(Solver::IResultWriter::Ptr writer, const Area& area)
{
    genericStoreTimeseriesNumbers(writer, area.wind.series->timeseriesNumbers, area.id, "wind");
}

void storeTimeseriesNumbersForThermal(Solver::IResultWriter::Ptr writer, const Area& area)
{
    area.thermal.list.storeTimeseriesNumbers(writer);
    area.thermal.mustrunList.storeTimeseriesNumbers(writer);
}

void storeTimeseriesNumbersForRenewable(Solver::IResultWriter::Ptr writer, const Area& area)
{
    area.renewable.list.storeTimeseriesNumbers(writer);
}

void storeTimeseriesNumbersForTransmissionCapacities(Solver::IResultWriter::Ptr writer, const Area& area)
{
    // No links originating from this area
    // do not create an empty directory
    if (area.links.empty())
        return;

    for (const auto& [key, value] : area.links)
    {
        if (value == nullptr)
        {
            logs.error() << "Unexpected nullptr encountered for area " << area.id;
            return;
        }
        else
            value->storeTimeseriesNumbers(writer);
    }
}
} // namespace Antares::Data

