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
#include <i_writer.h>
#include "../study.h"
#include "../../logs.h"

using namespace Yuni;

#define SEP IO::Separator
namespace Antares
{
namespace Data
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
                                   true,     // print_dimensions
                                   predicate, // predicate
                                   true);     // save even if all coeffs are zero

    writer->addEntryFromBuffer(path.c_str(), buffer);
}

void Area::storeTimeseriesNumbersForLoad(Solver::IResultWriter::Ptr writer) const
{
    genericStoreTimeseriesNumbers(writer, load.series->timeseriesNumbers, id, "load");
}

void Area::storeTimeseriesNumbersForSolar(Solver::IResultWriter::Ptr writer) const
{
    genericStoreTimeseriesNumbers(writer, solar.series->timeseriesNumbers, id, "solar");
}

void Area::storeTimeseriesNumbersForHydro(Solver::IResultWriter::Ptr writer) const
{
    genericStoreTimeseriesNumbers(writer, hydro.series->timeseriesNumbers, id, "hydro");
}

void Area::storeTimeseriesNumbersForWind(Solver::IResultWriter::Ptr writer) const
{
    genericStoreTimeseriesNumbers(writer, wind.series->timeseriesNumbers, id, "wind");
}

void Area::storeTimeseriesNumbersForThermal(Solver::IResultWriter::Ptr writer) const
{
    thermal.list.storeTimeseriesNumbers(writer);
    thermal.mustrunList.storeTimeseriesNumbers(writer);
}

void Area::storeTimeseriesNumbersForRenewable(Solver::IResultWriter::Ptr writer) const
{
    renewable.list.storeTimeseriesNumbers(writer);
}

void Area::storeTimeseriesNumbersForTransmissionCapacities(Solver::IResultWriter::Ptr writer) const
{
    // No links originating from this area
    // do not create an empty directory
    if (links.empty())
        return;

    for (const auto& link : links)
    {
        if (link.second == nullptr)
        {
            logs.error() << "Unexpected nullptr encountered for area " << id;
            return;
        }
        else
            link.second->storeTimeseriesNumbers(writer);
    }
}
} // namespace Data
} // namespace Antares
