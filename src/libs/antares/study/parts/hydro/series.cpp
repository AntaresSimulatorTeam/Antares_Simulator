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
#include <stdio.h>
#include "series.h"
#include <antares/inifile/inifile.h>
#include <antares/logs/logs.h>
#include "../../study.h"

using namespace Yuni;

#define SEP IO::Separator

namespace Antares
{
namespace Data
{
DataSeriesHydro::DataSeriesHydro() : count(0)
{
    // Pmin was introduced in v8.6
    // The previous behavior was Pmin=0
    // For compatibility reasons with existing studies, mingen, maxHourlyGenPower and maxHourlyPumpPower are set to one
    // column of zeros by default
    mingen.reset(1, HOURS_PER_YEAR);
    maxHourlyGenPower.reset(1, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(1, HOURS_PER_YEAR);
}

bool DataSeriesHydro::saveToFolder(const AreaName& areaID, const AnyString& folder) const
{
    String buffer;
    buffer.clear() << folder << SEP << areaID;
    /* Make sure the folder is created */
    if (IO::Directory::Create(buffer))
    {
        bool ret = true;

        // Saving data
        buffer.clear() << folder << SEP << areaID << SEP << "ror.txt";
        ret = ror.saveToCSVFile(buffer, 0) && ret;
        buffer.clear() << folder << SEP << areaID << SEP << "mod.txt";
        ret = storage.saveToCSVFile(buffer, 0) && ret;
        buffer.clear() << folder << SEP << areaID << SEP << "mingen.txt";
        ret = mingen.saveToCSVFile(buffer, 0) && ret;
        buffer.clear() << folder << SEP << areaID << SEP << "maxHourlyGenPower.txt";
        ret = maxHourlyGenPower.saveToCSVFile(buffer, 0) && ret;
        buffer.clear() << folder << SEP << areaID << SEP << "maxHourlyPumpPower.txt";
        ret = maxHourlyPumpPower.saveToCSVFile(buffer, 0) && ret;
        return ret;
    }
    return false;
}

bool DataSeriesHydro::loadFromFolder(Study& study, const AreaName& areaID, const AnyString& folder)
{
    bool ret = true;
    auto& buffer = study.bufferLoadingTS;

    buffer.clear() << folder << SEP << areaID << SEP << "ror." << study.inputExtension;

    ret = ror.loadFromCSVFile(buffer, 1, HOURS_PER_YEAR, &study.dataBuffer) && ret;

    buffer.clear() << folder << SEP << areaID << SEP << "mod." << study.inputExtension;
    ret = storage.loadFromCSVFile(buffer, 1, DAYS_PER_YEAR, &study.dataBuffer) && ret;

    // The number of time-series
    count = storage.width;

    if (ror.width > count)
        count = ror.width;

    if (study.header.version >= 860)
    {
        buffer.clear() << folder << SEP << areaID << SEP << "mingen." << study.inputExtension;
        ret = mingen.loadFromCSVFile(buffer, 1, HOURS_PER_YEAR, &study.dataBuffer) && ret;
    }

    if (study.usedByTheSolver)
    {
        if (0 == count)
        {
            logs.error() << "Hydro: `" << areaID
                         << "`: empty matrix detected. Fixing it with default values";
            ror.reset(1, HOURS_PER_YEAR);
            storage.reset(1, DAYS_PER_YEAR);
            mingen.reset(1, HOURS_PER_YEAR);
        }
        else
        {
            if (count > 1 && storage.width != ror.width)
            {
                if (ror.width != 1 && storage.width != 1)
                {
                    logs.fatal() << "Hydro: `" << areaID
                                 << "`: The matrices ROR (run-of-the-river) and hydro-storage must "
                                    "have the same number of time-series.";
                    study.gotFatalError = true;
                }
                else
                {
                    if (ror.width == 1)
                    {
                        ror.resizeWithoutDataLost(count, ror.height);
                        for (uint x = 1; x < count; ++x)
                            ror.pasteToColumn(x, ror[0]);
                    }
                    else
                    {
                        if (storage.width == 1)
                        {
                            storage.resizeWithoutDataLost(count, storage.height);
                            for (uint x = 1; x < count; ++x)
                                storage.pasteToColumn(x, storage[0]);
                        }
                    }
                    Area* areaToInvalidate = study.areas.find(areaID);
                    if (areaToInvalidate)
                    {
                        areaToInvalidate->invalidateJIT = true;
                        logs.info()
                          << "  '" << areaID << "': The hydro data have been normalized to "
                          << count << " timeseries";
                    }
                    else
                        logs.error()
                          << "Impossible to find the area `" << areaID << "` to invalidate it";
                }
            }
            checkMinGenTsNumber(study, areaID);
        }

        if (study.parameters.derated)
        {
            ror.averageTimeseries();
            storage.averageTimeseries();
            mingen.averageTimeseries();
            count = 1;
        }
    }

    timeseriesNumbers.clear();

    return ret;
}

void DataSeriesHydro::checkMinGenTsNumber(Study& study, const AreaName& areaID)
{
    if (mingen.width != storage.width)
    {
        if (mingen.width > 1)
        {
            logs.fatal() << "Hydro: `" << areaID
                         << "`: The matrices Minimum Generation must "
                            "has the same number of time-series as ROR and hydro-storage.";
            study.gotFatalError = true;
        }
        else
        {
            mingen.resizeWithoutDataLost(count, mingen.height);
            for (uint x = 1; x < count; ++x)
                mingen.pasteToColumn(x, mingen[0]);
            Area* areaToInvalidate = study.areas.find(areaID);
            if (areaToInvalidate)
            {
                areaToInvalidate->invalidateJIT = true;
                logs.info() << "  '" << areaID
                            << "': The hydro minimum generation data have been normalized to "
                            << count << " timeseries";
            }
            else
                logs.error() << "Impossible to find the area `" << areaID << "` to invalidate it";
        }
    }
}

bool DataSeriesHydro::forceReload(bool reload) const
{
    bool ret = true;
    ret = ror.forceReload(reload) && ret;
    ret = storage.forceReload(reload) && ret;
    ret = mingen.forceReload(reload) && ret;
    ret = maxHourlyGenPower.forceReload(reload) && ret;
    ret = maxHourlyPumpPower.forceReload(reload) && ret;
    return ret;
}

void DataSeriesHydro::markAsModified() const
{
    ror.markAsModified();
    storage.markAsModified();
    mingen.markAsModified();
    maxHourlyGenPower.markAsModified();
    maxHourlyPumpPower.markAsModified();
}

void DataSeriesHydro::reset()
{
    ror.reset(1, HOURS_PER_YEAR);
    storage.reset(1, DAYS_PER_YEAR);
    mingen.reset(1, HOURS_PER_YEAR);
    maxHourlyGenPower.reset(1, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(1, HOURS_PER_YEAR);
    count = 1;
    countpowercredits = 1;
}

uint64_t DataSeriesHydro::memoryUsage() const
{
    return sizeof(double) + ror.memoryUsage() + storage.memoryUsage() + mingen.memoryUsage()
           + maxHourlyGenPower.memoryUsage() + maxHourlyPumpPower.memoryUsage();
}

bool DataSeriesHydro::LoadMaxPower(const AreaName& areaID, const AnyString& folder)
{
    bool ret = true;
    YString filepath;
    Matrix<>::BufferType fileContent;

    filepath.clear() << folder << SEP << areaID << SEP << "maxHourlyGenPower"
                     << ".txt";
    ret = maxHourlyGenPower.loadFromCSVFile(filepath, 1, HOURS_PER_YEAR, &fileContent) && ret;
    filepath.clear() << folder << SEP << areaID << SEP << "maxHourlyPumpPower"
                     << ".txt";
    ret = maxHourlyPumpPower.loadFromCSVFile(filepath, 1, HOURS_PER_YEAR, &fileContent) && ret;

    timeseriesNumbersHydroMaxPower.clear();

    return ret;
}

bool DataSeriesHydro::postProcessMaxPowerTS(Area& area)
{
    NbTsComparer nbTSCompare(maxHourlyGenPower.width, maxHourlyPumpPower.width);
    TsActions tsActions(maxHourlyGenPower, maxHourlyPumpPower);
    //  What will happen if one width is 0 and second one is 1
    //  This case is not cover even in previous version
    if (nbTSCompare.bothZeros())
    {
        tsActions.handleBothZeros(area.id);
        return false;
    }

    if (nbTSCompare.same())
        return true;

    if (nbTSCompare.differentAndGreaterThanOne(countpowercredits))
        tsActions.handleBothGreaterThanOne(area.id);

    tsActions.resizeWhenOneTS(area, countpowercredits);

    return true;
}

void DataSeriesHydro::setHydroModulability(Study& study, const AreaName& areaID) const
{
    Area* area = study.areas.find(areaID);

    if (MatrixTestForAtLeastOnePositiveValue(maxHourlyGenPower))
    {
        area->hydro.hydroModulable = true;
    }
}

void DataSeriesHydro::setCountVariable()
{
    const auto& maxHourlyGenPower_ = maxHourlyGenPower.width;
    const auto& maxHourlyPumpPower_ = maxHourlyPumpPower.width;

    countpowercredits
      = (maxHourlyGenPower_ >= maxHourlyPumpPower_) ? maxHourlyGenPower_ : maxHourlyPumpPower_;
}

void DataSeriesHydro::setMaxPowerTSWhenDeratedMode(const Study& study)
{
    if (study.parameters.derated)
    {
        maxHourlyGenPower.averageTimeseries();
        maxHourlyPumpPower.averageTimeseries();
        countpowercredits = 1;
    }
}

DataSeriesHydro::NbTsComparer::NbTsComparer(uint32_t nbOfGenPowerTs_, uint32_t nbOfPumpPowerTs_) :
 nbOfGenPowerTs(nbOfGenPowerTs_), nbOfPumpPowerTs(nbOfPumpPowerTs_)
{
}

bool DataSeriesHydro::NbTsComparer::bothZeros() const
{
    return (nbOfGenPowerTs || nbOfPumpPowerTs) ? false : true;
}

bool DataSeriesHydro::NbTsComparer::same() const
{
    return (nbOfGenPowerTs == nbOfPumpPowerTs) ? true : false;
}

bool DataSeriesHydro::NbTsComparer::differentAndGreaterThanOne(uint countpowercredits_) const
{
    return (countpowercredits_ > 1 && (nbOfGenPowerTs != 1) && (nbOfPumpPowerTs != 1)) ? true
                                                                                       : false;
}

DataSeriesHydro::TsActions::TsActions(Matrix<double, int32_t>& maxHourlyGenPower_,
                                      Matrix<double, int32_t>& maxHourlyPumpPower_) :
 maxHourlyGenPower(maxHourlyGenPower_), maxHourlyPumpPower(maxHourlyPumpPower_)
{
}

void DataSeriesHydro::TsActions::handleBothZeros(const AreaName& areaID)
{
    logs.error() << "Hydro Max Power: `" << areaID
                 << "`: empty matrix detected. Fixing it with default values";

    maxHourlyGenPower.reset(1, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(1, HOURS_PER_YEAR);
}

[[noreturn]] void DataSeriesHydro::TsActions::handleBothGreaterThanOne(const AreaName& areaID) const
{
    logs.fatal() << "Hydro Max Power: `" << areaID
                 << "`: The matrices Maximum Generation and Maximum Pumping must "
                    "have the same number of time-series.";
    throw Error::ReadingStudy();
}

void DataSeriesHydro::TsActions::resizeWhenOneTS(Area& area, uint countpowercredits_)
{
    if (maxHourlyGenPower.width == 1)
    {
        resizeMatrixNoDataLoss(maxHourlyGenPower, countpowercredits_);
        areaToInvalidate(&area, area.id, countpowercredits_);
        return;
    }

    if (maxHourlyPumpPower.width == 1)
    {
        resizeMatrixNoDataLoss(maxHourlyPumpPower, countpowercredits_);
        areaToInvalidate(&area, area.id, countpowercredits_);
        return;
    }
}

void DataSeriesHydro::TsActions::areaToInvalidate(Area* area,
                                                  const AreaName& areaID,
                                                  uint countpowercredits) const
{
    if (area)
    {
        area->invalidateJIT = true;
        logs.info() << "  '" << area->id << "': The hydro max power data have been normalized to "
                    << countpowercredits << " timeseries";
    }
    else
        logs.error() << "Impossible to find the area `" << areaID << "` to invalidate it";
}

void resizeMatrixNoDataLoss(Matrix<double, int32_t>& matrixToResize, uint width)
{
    matrixToResize.resizeWithoutDataLost(width, matrixToResize.height);
    for (uint x = 1; x < width; ++x)
        matrixToResize.pasteToColumn(x, matrixToResize[0]);
}

} // namespace Data
} // namespace Antares
