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
#include "pair-of-integers.h"

using namespace Yuni;

#define SEP IO::Separator

namespace Antares
{
namespace Data
{
DataSeriesHydro::DataSeriesHydro()
{
    // Pmin was introduced in v8.6
    // The previous behavior was Pmin=0
    // For compatibility reasons with existing studies, mingen, maxHourlyGenPower and maxHourlyPumpPower are set to one
    // column of zeros by default
    mingen.reset(1, HOURS_PER_YEAR);
    maxHourlyGenPower.reset(1, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(1, HOURS_PER_YEAR);
}

void DataSeriesHydro::copyGenerationTS(DataSeriesHydro& source)
{
    ror = source.ror;
    storage = source.storage;
    mingen = source.mingen;

    generationTScount_ = source.generationTScount_;

    source.ror.unloadFromMemory();
    source.storage.unloadFromMemory();
    source.mingen.unloadFromMemory();
}
void DataSeriesHydro::copyMaxPowerTS(DataSeriesHydro& source)
{
    maxHourlyGenPower = source.maxHourlyGenPower;
    maxHourlyPumpPower = source.maxHourlyPumpPower;

    maxPowerTScount_ = source.maxPowerTScount_;

    source.maxHourlyGenPower.unloadFromMemory();
    source.maxHourlyPumpPower.unloadFromMemory();
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


bool loadTSfromFile(Matrix<double, int32_t>& ts, 
                    AreaName& areaID, 
                    const AnyString& folder, 
                    std::string filename,
                    unsigned int height)
{
    YString filePath;
    Matrix<>::BufferType fileContent;
    filePath.clear() << folder << SEP << areaID << SEP << filename;
    return ts.loadFromCSVFile(filePath, 1, HOURS_PER_YEAR, &fileContent);
}

bool DataSeriesHydro::loadROR(AreaName& areaID, const AnyString& folder)
{
    return loadTSfromFile(ror, areaID, folder, "ror.txt", HOURS_PER_YEAR);
}

bool DataSeriesHydro::loadINFLOWS(AreaName& areaID, const AnyString& folder)
{
    return loadTSfromFile(storage, areaID, folder, "mod.txt", DAYS_PER_YEAR);
}

bool DataSeriesHydro::loadMINGEN(AreaName& areaID, const AnyString& folder, unsigned int studyVersion)
{
    if (studyVersion < 860)
        return true;
    return loadTSfromFile(mingen, areaID, folder, "mingen.txt", HOURS_PER_YEAR);
}

bool DataSeriesHydro::loadFromFolder(Study& study, const AreaName& areaID, const AnyString& folder)
{
    // The number of time-series
    generationTScount_ = storage.width;

    if (ror.width > generationTScount_)
        generationTScount_ = ror.width;

    timeseriesNumbers.clear();

    if (!study.usedByTheSolver)
        return true;

    if (generationTScount_ > 1 && storage.width != ror.width)
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
                ror.resizeWithoutDataLost(generationTScount_, ror.height);
                for (uint x = 1; x < generationTScount_; ++x)
                    ror.pasteToColumn(x, ror[0]);
            }
            else
            {
                if (storage.width == 1)
                {
                    storage.resizeWithoutDataLost(generationTScount_, storage.height);
                    for (uint x = 1; x < generationTScount_; ++x)
                        storage.pasteToColumn(x, storage[0]);
                }
            }
            Area* areaToInvalidate = study.areas.find(areaID);
            if (areaToInvalidate)
            {
                areaToInvalidate->invalidateJIT = true;
                logs.info()
                    << "  '" << areaID << "': The hydro data have been normalized to "
                    << generationTScount_ << " timeseries";
            }
            else
                logs.error()
                    << "Impossible to find the area `" << areaID << "` to invalidate it";
        }
    }

    //std::string fatalErrorMsg = "Hydro : area `" + areaID.to<std::string>() + "` : ";
    //fatalErrorMsg += "ROR and INFLOWS must have the same number of time series.";

    //maxPowerTScount_ = EqualizeTSsize(maxHourlyGenPower, maxHourlyPumpPower, fatalError, fatalErrorMsg, area);

    //logs.info() << "  '" << area.id << "': The number of hydro max power (generation and pumping) "
    //    << "TS were both set to : " << maxPowerTScount_;
    
    checkMinGenTsNumber(study, areaID);

    if (study.parameters.derated)
    {
        ror.averageTimeseries();
        storage.averageTimeseries();
        mingen.averageTimeseries();
        generationTScount_ = 1;
    }

    return true;
}

bool DataSeriesHydro::LoadMaxPower(const AreaName& areaID, const AnyString& folder)
{
    bool ret = true;
    YString filepath;
    Matrix<>::BufferType fileContent;

    filepath.clear() << folder << SEP << areaID << SEP << "maxHourlyGenPower.txt";
    ret = maxHourlyGenPower.loadFromCSVFile(filepath, 1, HOURS_PER_YEAR, &fileContent) && ret;

    filepath.clear() << folder << SEP << areaID << SEP << "maxHourlyPumpPower.txt";
    ret = maxHourlyPumpPower.loadFromCSVFile(filepath, 1, HOURS_PER_YEAR, &fileContent) && ret;

    timeseriesNumbersHydroMaxPower.clear();

    return ret;
}


void ConvertDailyTSintoHourlyTS(const Matrix<double>::ColumnType& dailyColumn,
                                Matrix<double, int32_t>::ColumnType& hourlyColumn)
{
    uint hour = 0;
    uint day = 0;

    while (hour < HOURS_PER_YEAR && day < DAYS_PER_YEAR)
    {
        for (uint i = 0; i < HOURS_PER_DAY; ++i)
        {
            hourlyColumn[hour] = dailyColumn[day];
            ++hour;
        }
        ++day;
    }
}


void DataSeriesHydro::buildMaxPowerFromDailyTS(const Matrix<double>::ColumnType& DailyMaxGenPower,
                                               const Matrix<double>::ColumnType& DailyMaxPumpPower)
{
    maxPowerTScount_ = 1;

    maxHourlyGenPower.reset(maxPowerTScount_, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(maxPowerTScount_, HOURS_PER_YEAR);

    ConvertDailyTSintoHourlyTS(DailyMaxGenPower, maxHourlyGenPower[0]);
    ConvertDailyTSintoHourlyTS(DailyMaxPumpPower, maxHourlyPumpPower[0]);
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
            mingen.resizeWithoutDataLost(generationTScount_, mingen.height);
            for (uint x = 1; x < generationTScount_; ++x)
                mingen.pasteToColumn(x, mingen[0]);
            Area* areaToInvalidate = study.areas.find(areaID);
            if (areaToInvalidate)
            {
                areaToInvalidate->invalidateJIT = true;
                logs.info() << "  '" << areaID
                            << "': The hydro minimum generation data have been normalized to "
                            << generationTScount_ << " timeseries";
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
    generationTScount_ = 1;
    maxPowerTScount_ = 1;
}

void DataSeriesHydro::resizeRORandSTORAGE(unsigned int width)
{
    ror.resize(width, HOURS_PER_YEAR);
    storage.resize(width, DAYS_PER_YEAR);
    generationTScount_ = width;
}

void DataSeriesHydro::resizeGenerationTS(unsigned int w, unsigned int h)
{
    ror.resize(w, h);
    storage.resize(w, h);
    mingen.resize(w, h);
    generationTScount_ = w;
}

void DataSeriesHydro::resizeMaxPowerTS(unsigned int w, unsigned int h)
{
    maxHourlyGenPower.reset(w, h);
    maxHourlyPumpPower.reset(w, h);
    maxPowerTScount_ = w;
}

uint64_t DataSeriesHydro::memoryUsage() const
{
    return sizeof(double) + ror.memoryUsage() + storage.memoryUsage() + mingen.memoryUsage()
           + maxHourlyGenPower.memoryUsage() + maxHourlyPumpPower.memoryUsage();
}

unsigned int EqualizeTSsize(Matrix<double, int32_t>& TScollection1, 
                        Matrix<double, int32_t>& TScollection2, 
                        bool& fatalError,
                        std::string fatalErrorMsg,
                        Area& area)
{
    PairOfIntegers pairOfTSsizes(TScollection1.width, TScollection2.width);

    if (pairOfTSsizes.same())
        return pairOfTSsizes.sup();

    if (pairOfTSsizes.bothGreaterThanOne())
    {
        logs.fatal() << fatalErrorMsg;
        fatalError = true;
        return 0;
    }

    // At this point, one TS collection size is > 1 and the other is of size 1.
    
    // This following instruction to force reloading all area's TS when saving the study (GUI)
    area.invalidateJIT = true;

    if (TScollection1.width == 1)
        resizeMatrixNoDataLoss(TScollection1, pairOfTSsizes.sup());
    if (TScollection2.width == 1)
        resizeMatrixNoDataLoss(TScollection2, pairOfTSsizes.sup());

    return pairOfTSsizes.sup();
}

void DataSeriesHydro::EqualizeMaxPowerTSsizes(Area& area, bool& fatalError)
{
    std::string fatalErrorMsg = "Hydro Max Power: " + area.id.to<std::string>() + " : ";
    fatalErrorMsg += "generation and pumping must have the same number of TS.";

    maxPowerTScount_ = EqualizeTSsize(maxHourlyGenPower, maxHourlyPumpPower, fatalError, fatalErrorMsg, area);

    logs.info() << "  '" << area.id << "': The number of hydro max power (generation and pumping) " 
                << "TS were both set to : " << maxPowerTScount_;
}

void DataSeriesHydro::setHydroModulability(Study& study, const AreaName& areaID) const
{
    Area* area = study.areas.find(areaID);

    if (MatrixTestForAtLeastOnePositiveValue(maxHourlyGenPower))
    {
        area->hydro.hydroModulable = true;
    }
}

uint DataSeriesHydro::maxPowerTScount() const
{
    return maxPowerTScount_;
}

void DataSeriesHydro::setMaxPowerTSWhenDeratedMode(const Study& study)
{
    if (study.parameters.derated)
    {
        maxHourlyGenPower.averageTimeseries();
        maxHourlyPumpPower.averageTimeseries();
        maxPowerTScount_ = 1;
    }
}

// TODO : this function should not be here, as it applies to 
//        any time series, not just hydro TS.
void resizeMatrixNoDataLoss(Matrix<double, int32_t>& matrixToResize, uint width)
{
    matrixToResize.resizeWithoutDataLost(width, matrixToResize.height);
    for (uint x = 1; x < width; ++x)
        matrixToResize.pasteToColumn(x, matrixToResize[0]);
}

} // namespace Data
} // namespace Antares
