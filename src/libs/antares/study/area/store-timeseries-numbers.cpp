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
#include "../study.h"
#include "../../logs.h"

using namespace Yuni;

#define SEP IO::Separator
namespace {
    static const YString DIRECTORY_NAME_FOR_TRANSMISSION_CAPACITIES = "ntc";
}

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

bool Area::storeTimeseriesNumbersForLoad(Study& study)
{
    study.buffer.clear() << study.folderOutput << SEP << "ts-numbers" << SEP << "load";
    if (!IO::Directory::Create(study.buffer))
    {
        logs.error() << "I/O Error: impossible to create the folder " << study.buffer;
        return false;
    }
    TSNumbersPredicate predicate;
    study.buffer << SEP << this->id << ".txt";
    return load.series->timeseriesNumbers.saveToCSVFile(study.buffer, 0, true, predicate);
}

bool Area::storeTimeseriesNumbersForSolar(Study& study)
{
    if (study.header.version >= 330)
    {
        // Before 330, the folder for the solar time-series does not exist
        study.buffer.clear() << study.folderOutput << SEP << "ts-numbers" << SEP << "solar";

        if (!IO::Directory::Create(study.buffer))
        {
            logs.error() << "I/O Error: impossible to create the folder " << study.buffer;
            return false;
        }
        study.buffer << SEP << this->id << ".txt";

        TSNumbersPredicate predicate;
        return solar.series->timeseriesNumbers.saveToCSVFile(study.buffer, 0, true, predicate);
    }
    return true;
}

bool Area::storeTimeseriesNumbersForHydro(Study& study)
{
    study.buffer.clear() << study.folderOutput << SEP << "ts-numbers" << SEP << "hydro";

    if (!IO::Directory::Create(study.buffer))
    {
        logs.error() << "I/O Error: impossible to create the folder " << study.buffer;
        return false;
    }

    TSNumbersPredicate predicate;
    study.buffer << SEP << this->id << ".txt";
    return hydro.series->timeseriesNumbers.saveToCSVFile(study.buffer, 0, true, predicate);
}

bool Area::storeTimeseriesNumbersForWind(Study& study)
{
    study.buffer.clear() << study.folderOutput << SEP << "ts-numbers" << SEP << "wind";

    if (!IO::Directory::Create(study.buffer))
    {
        logs.error() << "I/O Error: impossible to create the folder " << study.buffer;
        return false;
    }

    TSNumbersPredicate predicate;
    study.buffer << SEP << this->id << ".txt";
    return wind.series->timeseriesNumbers.saveToCSVFile(study.buffer, 0, true, predicate);
}

bool Area::storeTimeseriesNumbersForThermal(Study& study)
{
    study.buffer.clear() << study.folderOutput << SEP << "ts-numbers" << SEP
                         << thermal.list.typeID() << SEP << id;

    if (!IO::Directory::Create(study.buffer))
    {
        logs.error() << "I/O Error: impossible to create the folder " << study.buffer;
        return false;
    }

    bool ret = thermal.list.storeTimeseriesNumbers(study);
    ret = thermal.mustrunList.storeTimeseriesNumbers(study) && ret;
    return ret;
}

bool Area::storeTimeseriesNumbersForRenewable(Study& study)
{
    study.buffer.clear() << study.folderOutput << SEP << "ts-numbers" << SEP
                         << renewable.list.typeID() << SEP << id;

    if (!IO::Directory::Create(study.buffer))
    {
        logs.error() << "I/O Error: impossible to create the folder " << study.buffer;
        return false;
    }

    bool ret = renewable.list.storeTimeseriesNumbers(study);
    return ret;
}

bool Area::storeTimeseriesNumbersForTransmissionCapacities(Study& study)
{
    // No links originating from this area
    // do not create an empty directory
    if (links.empty())
        return true;

    study.buffer.clear() << study.folderOutput << SEP << "ts-numbers" << SEP
                         << DIRECTORY_NAME_FOR_TRANSMISSION_CAPACITIES << SEP << id;

    if (!IO::Directory::Create(study.buffer))
    {
        logs.error() << "I/O Error: impossible to create the folder " << study.buffer;
        return false;
    }

    bool ret = true;
    for (const auto& link : links)
    {
        if (link.second == nullptr)
        {
            logs.error() << "Unexpected nullptr encountered for area " << id;
            return false;
        }
        else
        {
            ret = link.second->storeTimeseriesNumbers(study.buffer) && ret;
        }
    }
    return ret;
}
} // namespace Data
} // namespace Antares
