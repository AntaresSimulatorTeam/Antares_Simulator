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

#include <yuni/yuni.h>
#include <yuni/core/math.h>
#include <yuni/io/directory.h>
#include <yuni/io/file.h>

#include <antares/logs/logs.h>
#include <antares/solver/ts-generator/prepro.h>
#include "antares/study/study.h"

using namespace Yuni;

#define SEP IO::Separator

namespace Antares::Data
{
PreproAvailability::PreproAvailability(const YString& id, unsigned int unitCount):
    id(id),
    unitCount(unitCount)
{
}

void PreproAvailability::copyFrom(const PreproAvailability& rhs)
{
    id = rhs.id;
    unitCount = rhs.unitCount;
    data = rhs.data;
    rhs.data.unloadFromMemory();
}

bool PreproAvailability::saveToFolder(const AnyString& folder) const
{
    if (IO::Directory::Create(folder))
    {
        String buffer;
        buffer.clear() << folder << SEP << "data.txt";
        return data.saveToCSVFile(buffer, /*decimal*/ 6);
    }
    return false;
}

bool PreproAvailability::loadFromFolder(Study& study, const std::filesystem::path& folder)
{
    auto filePath = folder / "data.txt";
    // standard loading
    return data.loadFromCSVFile(filePath.string(),
                                preproAvailabilityMax,
                                DAYS_PER_YEAR,
                                Matrix<>::optFixedSize,
                                &study.dataBuffer);
}

bool PreproAvailability::validate() const
{
    const auto& colFoRate = data[foRate];
    const auto& colPoRate = data[poRate];
    const auto& colFoDuration = data[foDuration];
    const auto& colPoDuration = data[poDuration];
    const auto& colNPOMin = data[npoMin];
    const auto& colNPOMax = data[npoMax];
    uint errors = 0;

    for (uint i = 0; i != DAYS_PER_YEAR; ++i)
    {
        double foRate = colFoRate[i];
        double poRate = colPoRate[i];
        double foDuration = colFoDuration[i];
        double poDuration = colPoDuration[i];
        double cNPOMin = colNPOMin[i];
        double cNPOMax = colNPOMax[i];

        if (cNPOMin < 0)
        {
            logs.error() << "Prepro: " << id << ": NPO min can not be negative (line:" << (i + 1)
                         << ")";
            ++errors;
        }
        if (cNPOMax < 0)
        {
            logs.error() << "Prepro: " << id << ": NPO max can not be negative (line:" << (i + 1)
                         << ")";
            ++errors;
        }
        if (cNPOMin > cNPOMax)
        {
            logs.error() << "Prepro: " << id
                         << ": NPO max must be greater or equal to NPO min (line:" << (i + 1)
                         << ")";
            ++errors;
        }

        if (foRate < 0. || foRate > 1.)
        {
            logs.error() << "Prepro: " << id << ": invalid value for FO rate (line:" << (i + 1)
                         << ")";
            ++errors;
        }

        if (poRate < 0. || poRate > 1.)
        {
            logs.error() << "Prepro: " << id << ": invalid value for PO rate (line:" << (i + 1)
                         << ")";
            ++errors;
        }

        if (foDuration < 1. || foDuration > 365.)
        {
            logs.error() << "Prepro: " << id << ": invalid value for FO Duration (line:" << (i + 1)
                         << ")";
            ++errors;
        }
        if (poDuration < 1. || poDuration > 365.)
        {
            logs.error() << "Prepro: " << id << ": invalid value for PO Duration (line:" << (i + 1)
                         << ")";
            ++errors;
        }

        if (errors > 30)
        {
            logs.error() << "Prepro: " << id << ": too many errors. skipping";
            break;
        }
    }
    return errors == 0;
}

bool PreproAvailability::forceReload(bool reload) const
{
    return data.forceReload(reload);
}

void PreproAvailability::markAsModified() const
{
    data.markAsModified();
}

uint64_t PreproAvailability::memoryUsage() const
{
    return sizeof(PreproAvailability);
}

void PreproAvailability::reset()
{
    data.reset(preproAvailabilityMax, DAYS_PER_YEAR, true);

    auto& colFoDuration = data[foDuration];
    auto& colPoDuration = data[poDuration];

    for (uint i = 0; i != DAYS_PER_YEAR; ++i)
    {
        colFoDuration[i] = 1.;
        colPoDuration[i] = 1.;
    }
}

bool PreproAvailability::normalizeAndCheckNPO()
{
    // alias to our data columns
    auto& columnNPOMax = data[npoMax];
    auto& columnNPOMin = data[npoMin];
    // errors management
    uint errors = 0;
    uint maxErrors = 10;

    // Flag to determine whether the column NPO max has been normalized or not
    bool normalized = false;

    for (uint y = 0; y != data.height; ++y)
    {
        if (columnNPOMax[y] > unitCount)
        {
            columnNPOMax[y] = unitCount;
            normalized = true;
        }

        if (columnNPOMin[y] > columnNPOMax[y] && ++errors < maxErrors)
        {
            logs.error() << id << ": NPO min can not be greater than NPO max (hour: " << (y + 1)
                         << ", npo-min: " << columnNPOMin[y] << ", npo-max: " << columnNPOMax[y]
                         << ')';
        }
    }

    if (errors >= maxErrors)
    {
        logs.error() << id << ": too many errors. skipping (total: " << errors << ')';
    }

    if (normalized)
    {
        logs.info() << "  NPO max for entity '" << id << "' has been normalized";
    }

    data.markAsModified();
    return (0 == errors);
}

} // namespace Antares::Data
