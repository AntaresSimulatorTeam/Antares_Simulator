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

#include "TSnumberData.h"
#include "scBuilderUtils.h"

namespace Antares
{
namespace Data
{
namespace ScenarioBuilder
{
enum
{
    maxErrors = 20,
};

bool TSNumberData::reset(const Study& study)
{
    assert(&study != nullptr);

    const uint nbYears = study.parameters.nbYears;

    // Standard timeseries (load, wind, ...)
    pTSNumberRules.reset(study.areas.size(), nbYears);
    return true;
}

void TSNumberData::saveToINIFile(const Study& study, Yuni::IO::File::Stream& file) const
{
    // Prefix
    CString<512, false> prefix;
    prefix += get_prefix();

    // Foreach year
    assert(pTSNumberRules.width == study.areas.size());
    for (uint index = 0; index != pTSNumberRules.width; ++index)
    {
        // alias to the current column
        const MatrixType::ColumnType& col = pTSNumberRules[index];
        // Foreach area...
        for (uint y = 0; y != pTSNumberRules.height; ++y)
        {
            const MatrixType::Type value = col[y];
            // Equals to zero means 'auto', which is the default mode
            if (!value)
                continue;
            assert(index < study.areas.size());
            file << prefix << study.areas.byIndex[index]->id << ',' << y << " = " << value << '\n';
        }
    }
}

void TSNumberData::set(uint areaindex, uint year, uint value)
{
    assert(areaindex < pTSNumberRules.width);
    if (year < pTSNumberRules.height)
        pTSNumberRules[areaindex][year] = value;
}

void TSNumberData::set_value(uint x, uint y, uint value)
{
    pTSNumberRules.entry[y][x] = value;
}

namespace // anonymous
{
template<class D>
static inline bool CheckValidity(uint value, const D& data, uint tsGenMax)
{
    // When the TS-Generators are not used
    return (!tsGenMax) ? (value < data.series.width) : (value < tsGenMax);
}

template<>
inline bool CheckValidity<Data::DataSeriesHydro>(uint value,
                                                 const Data::DataSeriesHydro& data,
                                                 uint tsGenMax)
{
    // When the TS-Generators are not used
    return (!tsGenMax) ? (value < data.count) : (value < tsGenMax);
}

template<>
inline bool CheckValidity<Data::AreaLink>(uint value,
                                          const Data::AreaLink& data,
                                          uint tsGenMax)
{
    return value < data.directCapacities.width;
}

template<class StringT, class D>
static bool ApplyToMatrix(uint& errors,
                          StringT& logprefix,
                          D& data,
                          const TSNumberData::MatrixType::ColumnType& years,
                          uint tsGenMax)
{
    bool status_to_return = true;

    // In this case, m.height represents the total number of years
    const uint nbYears = data.timeseriesNumbers.height;
    // The matrix m has only one column
    assert(data.timeseriesNumbers.width == 1);
    typename Matrix<uint32>::ColumnType& target = data.timeseriesNumbers[0];

    for (uint y = 0; y != nbYears; ++y)
    {
        if (years[y] != 0)
        {
            // The new TS number
            uint tsNum = years[y] - 1;

            // When the TS-Generators are not used
            if (!CheckValidity(tsNum, data, tsGenMax))
            {
                if (errors <= maxErrors)
                {
                    if (++errors == maxErrors)
                        logs.warning() << "scenario-builder: ... (skipped)";
                    else
                        logs.warning() << "scenario-builder: " << logprefix
                                       << "value out of bounds for the year " << (y + 1);
                }
                status_to_return = false;
                continue;
            }
            // Ok, assign. The value provided by the interface is user-friendly
            // and starts from 1.
            target[y] = tsNum;
        }
    }

    return status_to_return;
}

} // anonymous namespace

// =============== TSNumberData derived classes ===============

// ================================
// Load ...
// ================================
uint loadTSNumberData::get_tsGenCount(const Study& study) const
{
    // General data
    auto& parameters = study.parameters;

    const bool tsGenLoad = (0 != (parameters.timeSeriesToGenerate & timeSeriesLoad));
    return tsGenLoad ? parameters.nbTimeSeriesLoad : 0u;
}

bool loadTSNumberData::apply(Study& study)
{
    bool status_to_return = true;
    CString<512, false> logprefix;
    // Errors
    uint errors = 0;

    // The total number of areas;
    const uint areaCount = study.areas.size();

    const uint tsGenCountLoad = get_tsGenCount(study);

    for (uint areaIndex = 0; areaIndex != areaCount; ++areaIndex)
    {
        // Alias to the current area
        Area& area = *(study.areas.byIndex[areaIndex]);
        // alias to the current column
        assert(areaIndex < pTSNumberRules.width);
        const MatrixType::ColumnType& col = pTSNumberRules[areaIndex];

        logprefix.clear() << "Load: Area '" << area.name << "': ";
        status_to_return = status_to_return && ApplyToMatrix(errors, logprefix, *area.load.series, col, tsGenCountLoad);
    }
    return status_to_return;
}


// ================================
// Wind ...
// ================================

uint windTSNumberData::get_tsGenCount(const Study& study) const
{
    // General data
    auto& parameters = study.parameters;

    const bool tsGenWind = (0 != (parameters.timeSeriesToGenerate & timeSeriesWind));
    return tsGenWind ? parameters.nbTimeSeriesWind : 0u;
}

bool windTSNumberData::apply(/*const*/ Study& study)
{
    bool status_to_return = true;
    CString<512, false> logprefix;
    // Errors
    uint errors = 0;

    // The total number of areas;
    const uint areaCount = study.areas.size();

    const uint tsGenCountWind = get_tsGenCount(study);

    for (uint areaIndex = 0; areaIndex != areaCount; ++areaIndex)
    {
        // Alias to the current area
        Area& area = *(study.areas.byIndex[areaIndex]);
        // alias to the current column
        assert(areaIndex < pTSNumberRules.width);
        const MatrixType::ColumnType& col = pTSNumberRules[areaIndex];

        logprefix.clear() << "Wind: Area '" << area.name << "': ";
        status_to_return = status_to_return && ApplyToMatrix(errors, logprefix, *area.wind.series, col, tsGenCountWind);
    }
    return status_to_return;
}


// ================================
// Solar ...
// ================================

uint solarTSNumberData::get_tsGenCount(const Study& study) const
{
    // General data
    auto& parameters = study.parameters;

    const bool tsGenSolar = (0 != (parameters.timeSeriesToGenerate & timeSeriesSolar));
    return tsGenSolar ? parameters.nbTimeSeriesSolar : 0u;
}

bool solarTSNumberData::apply(Study& study)
{
    bool status_to_return = true;
    CString<512, false> logprefix;
    // Errors
    uint errors = 0;

    // The total number of areas;
    const uint areaCount = study.areas.size();

    const uint tsGenCountSolar = get_tsGenCount(study);

    for (uint areaIndex = 0; areaIndex != areaCount; ++areaIndex)
    {
        // Alias to the current area
        Area& area = *(study.areas.byIndex[areaIndex]);
        // alias to the current column
        assert(areaIndex < pTSNumberRules.width);
        const MatrixType::ColumnType& col = pTSNumberRules[areaIndex];

        logprefix.clear() << "Solar: Area '" << area.name << "': ";
        status_to_return = status_to_return && ApplyToMatrix(errors, logprefix, *area.solar.series, col, tsGenCountSolar);
    }
    return status_to_return;
}


// ================================
// Hydro ...
// ================================

uint hydroTSNumberData::get_tsGenCount(const Study& study) const
{
    // General data
    auto& parameters = study.parameters;

    const bool tsGenHydro = (0 != (parameters.timeSeriesToGenerate & timeSeriesHydro));
    return tsGenHydro ? parameters.nbTimeSeriesHydro : 0u;
}

bool hydroTSNumberData::apply(Study& study)
{
    bool status_to_return = true;
    CString<512, false> logprefix;
    // Errors
    uint errors = 0;

    // The total number of areas;
    const uint areaCount = study.areas.size();

    const uint tsGenCountHydro = get_tsGenCount(study);

    for (uint areaIndex = 0; areaIndex != areaCount; ++areaIndex)
    {
        // Alias to the current area
        Area& area = *(study.areas.byIndex[areaIndex]);
        // alias to the current column
        assert(areaIndex < pTSNumberRules.width);
        const MatrixType::ColumnType& col = pTSNumberRules[areaIndex];

        logprefix.clear() << "Hydro: Area '" << area.name << "': ";
        status_to_return = status_to_return && ApplyToMatrix(errors, logprefix, *area.hydro.series, col, tsGenCountHydro);
    }
    return status_to_return;
}


// ================================
// Thermal ...
// ================================

bool thermalTSNumberData::reset(const Study& study)
{
    assert(&study != nullptr);

    const uint nbYears = study.parameters.nbYears;
    assert(pArea != nullptr);

    // If an area is available, it can only be an overlay for thermal timeseries
    // WARNING: The total number of clusters may vary if used from the
    //   solver or not.
    // WARNING: At this point in time, the variable pArea->thermal.clusterCount()
    //   might not be valid (because not really initialized yet)
    uint clusterCount = (study.usedByTheSolver)
                          ? (pArea->thermal.list.size() + pArea->thermal.mustrunList.size())
                          : pArea->thermal.list.size();

    // Resize
    pTSNumberRules.reset(clusterCount, nbYears);
    return true;
}

void thermalTSNumberData::saveToINIFile(const Study& study, Yuni::IO::File::Stream& file) const
{
    // Prefix
    CString<512, false> prefix;
    prefix += get_prefix();

    if (!pArea)
        return;

#ifndef NDEBUG
    if (pTSNumberRules.width)
    {
        assert(pTSNumberRules.width == pArea->thermal.list.size());
    }
#endif

    // Foreach thermal cluster...
    for (uint index = 0; index != pTSNumberRules.width; ++index)
    {
        // Foreach year ...
        for (uint y = 0; y != pTSNumberRules.height; ++y)
        {
            const uint val = get(pArea->thermal.list.byIndex[index], y);
            // Equals to zero means 'auto', which is the default mode
            if (!val)
                continue;
            file << prefix << pArea->id << "," << y << ','
                 << pArea->thermal.list.byIndex[index]->id() << " = " << val << '\n';
        }
    }
}

void thermalTSNumberData::set(const Antares::Data::ThermalCluster* cluster,
                              const uint year,
                              uint value)
{
    assert(cluster != nullptr);
    if (clusterIndexMap.find(cluster) == clusterIndexMap.end())
        clusterIndexMap[cluster] = cluster->areaWideIndex;
    if (year < pTSNumberRules.height)
        pTSNumberRules[clusterIndexMap[cluster]][year] = value;
}

bool thermalTSNumberData::apply(Study& study)
{
    bool status_to_return = true;
    CString<512, false> logprefix;
    // Errors
    uint errors = 0;

    // Alias to the current area
    assert(pArea != nullptr);
    assert(pArea->index < study.areas.size());
    Area& area = *(study.areas.byIndex[pArea->index]);
    // The total number of clusters for the area
    // WARNING: We may have some thermal clusters with the `mustrun` option
    auto clusterCount = (uint)area.thermal.clusterCount();

    const uint tsGenCountThermal = get_tsGenCount(study);

    for (uint clusterIndex = 0; clusterIndex != clusterCount; ++clusterIndex)
    {
        auto& cluster = *(area.thermal.clusters[clusterIndex]);
        // alias to the current column
        assert(clusterIndex < pTSNumberRules.width);
        auto& col = pTSNumberRules[clusterIndex];

        logprefix.clear() << "Thermal: Area '" << area.name << "', cluster: '" << cluster.name()
                          << "': ";
        status_to_return = status_to_return && ApplyToMatrix(errors, logprefix, *cluster.series, col, tsGenCountThermal);
    }
    return status_to_return;
}

uint thermalTSNumberData::get_tsGenCount(const Study& study) const
{
    // General data
    auto& parameters = study.parameters;

    bool tsGenThermal = (0 != (parameters.timeSeriesToGenerate & timeSeriesThermal));
    return tsGenThermal ? parameters.nbTimeSeriesThermal : 0u;
}

// ================================
// Renewable clusters ...
// ================================
void renewableTSNumberData::set(const Antares::Data::RenewableCluster* cluster,
                                const uint year,
                                uint value)
{
    assert(cluster != nullptr);
    if (clusterIndexMap.find(cluster) == clusterIndexMap.end())
        clusterIndexMap[cluster] = cluster->areaWideIndex;
    if (year < pTSNumberRules.height)
        pTSNumberRules[clusterIndexMap[cluster]][year] = value;
}

bool renewableTSNumberData::apply(Study& study)
{
    bool status_to_return = true;
    CString<512, false> logprefix;
    // Errors
    uint errors = 0;

    // Alias to the current area
    assert(pArea != nullptr);
    assert(pArea->index < study.areas.size());
    Area& area = *(study.areas.byIndex[pArea->index]);
    // The total number of clusters for the area
    // WARNING: We may have some renewable clusters with the `mustrun` option
    auto clusterCount = (uint)area.renewable.clusterCount();

    const uint tsGenCountRenewable = get_tsGenCount(study);

    for (uint clusterIndex = 0; clusterIndex != clusterCount; ++clusterIndex)
    {
        auto& cluster = *(area.renewable.clusters[clusterIndex]);
        // alias to the current column
        assert(clusterIndex < pTSNumberRules.width);
        auto& col = pTSNumberRules[clusterIndex];

        logprefix.clear() << "Renewable: Area '" << area.name << "', cluster: '" << cluster.name()
                          << "': ";
        status_to_return = status_to_return && ApplyToMatrix(errors, logprefix, *cluster.series, col, tsGenCountRenewable);
    }
    return status_to_return;
}

uint renewableTSNumberData::get_tsGenCount(const Study& study) const
{
    // General data
    auto& parameters = study.parameters;

    bool tsGenRenewable = (0 != (parameters.timeSeriesToGenerate & timeSeriesRenewable));
    return tsGenRenewable ? 1 : 0u;
}

void renewableTSNumberData::saveToINIFile(const Study& study, Yuni::IO::File::Stream& file) const
{
    // Prefix
    CString<512, false> prefix;
    prefix += get_prefix();

    if (!pArea)
        return;

// Foreach year
#ifndef NDEBUG
    if (pTSNumberRules.width)
    {
        assert(pTSNumberRules.width == pArea->renewable.list.size());
    }
#endif

    for (uint index = 0; index != pTSNumberRules.width; ++index)
    {
        // Foreach renewable cluster...
        for (uint y = 0; y != pTSNumberRules.height; ++y)
        {
            const uint val = get(pArea->renewable.list.byIndex[index], y);
            // Equals to zero means 'auto', which is the default mode
            if (!val)
                continue;
            file << prefix << pArea->id << "," << y << ','
                 << pArea->renewable.list.byIndex[index]->id() << " = " << val << '\n';
        }
    }
}

bool renewableTSNumberData::reset(const Study& study)
{
    assert(&study != nullptr);

    const uint nbYears = study.parameters.nbYears;
    assert(pArea != nullptr);

    // If an area is available, it can only be an overlay for renewable timeseries
    // WARNING: The total number of clusters may vary if used from the
    //   solver or not.
    // WARNING: At this point in time, the variable pArea->renewable.clusterCount()
    //   might not be valid (because not really initialized yet)
    uint clusterCount
      = (study.usedByTheSolver) ? (pArea->renewable.list.size()) : pArea->renewable.list.size();
    // Resize
    pTSNumberRules.reset(clusterCount, nbYears);
    return true;
}


// ================================
// Transmission capacities ...
// ================================
bool ntcTSNumberData::reset(const Study& study)
{
    assert(&study != nullptr);

    const uint nbYears = study.parameters.nbYears;
    assert(pArea != nullptr);

    auto linkCount = (uint)pArea->links.size();

    // Resize
    pTSNumberRules.reset(linkCount, nbYears);
    return true;
}

void ntcTSNumberData::saveToINIFile(const Study& study, Yuni::IO::File::Stream& file) const
{
    if (!pArea)
        return;
    
    // Prefix
    CString<512, false> prefix;
    prefix += get_prefix();

#ifndef NDEBUG
    if (pTSNumberRules.width)
    {
        assert(pTSNumberRules.width == pArea->links.size());
    }
#endif

    for (auto i = pArea->links.begin(); i != pArea->links.end(); ++i)
    {
        auto* link = i->second;
        for (uint y = 0; y != pTSNumberRules.height; ++y)
        {
            const uint val = pTSNumberRules[link->indexForArea][y];
            // Equals to zero means 'auto', which is the default mode
            if (!val)
                continue;
            file << prefix << pArea->id << "," << i->first << "," << y << " = " << val << "\n";
        }
    }
}

void ntcTSNumberData::set(const Antares::Data::AreaLink* link,
                          const uint year,
                          uint value)
{
    assert(link != nullptr);
    if (linksIndexMap.find(link) == linksIndexMap.end())
        linksIndexMap[link] = link->indexForArea;
    if (year < pTSNumberRules.height)
        pTSNumberRules[linksIndexMap[link]][year] = value;
}

bool ntcTSNumberData::apply(Study& study)
{
    bool status_to_return = true;
    CString<512, false> logprefix;
    // Errors
    uint errors = 0;

    // Alias to the current area
    assert(pArea != nullptr);
    assert(pArea->index < study.areas.size());
    Area& area = *(study.areas.byIndex[pArea->index]);

    const uint ntcGeneratedTScount = get_tsGenCount(study);

    for (auto i = pArea->links.begin(); i != pArea->links.end(); ++i)
    {
        auto* link = i->second;
        uint linkIndex = link->indexForArea;
        assert(linkIndex < pTSNumberRules.width);
        auto& col = pTSNumberRules[linkIndex];
        logprefix.clear() << "NTC: Area '" << area.name << "', link: '" << link->getName() << "': ";
        status_to_return = status_to_return && ApplyToMatrix(errors, logprefix, *link, col, ntcGeneratedTScount);
    }
    return status_to_return;
}

uint ntcTSNumberData::get_tsGenCount(const Study& study) const
{
    return 0;
}

} // namespace ScenarioBuilder
} // namespace Data
} // namespace Antares
