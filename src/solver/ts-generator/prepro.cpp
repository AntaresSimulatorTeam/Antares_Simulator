// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <yuni/yuni.h>

#include <antares/logs/logs.h>
#include <antares/solver/ts-generator/prepro.h>
#include "antares/study/study.h"

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
    if (Yuni::IO::Directory::Create(folder))
    {
        Yuni::String buffer;
        buffer.clear() << folder << Yuni::IO::Separator << "data.txt";
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

        if (columnNPOMin[y] > columnNPOMax[y])
        {
            ++errors;
            if (errors < maxErrors)
            {
                logs.error() << id << ": NPO min can not be greater than NPO max (hour: " << (y + 1)
                             << ", npo-min: " << columnNPOMin[y] << ", npo-max: " << columnNPOMax[y]
                             << ')';
            }
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

    return (0 == errors);
}

} // namespace Antares::Data
