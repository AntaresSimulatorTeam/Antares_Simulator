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

#include "TSnumberData.h"
#include "scBuilderUtils.h"
#include "applyToMatrix.hxx"

namespace Antares::Data::ScenarioBuilder
{

bool TSNumberData::reset(const Study& study)
{
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
    bool ret = true;
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
        ret = ApplyToMatrix(errors, logprefix, *area.load.series, col, tsGenCountLoad) && ret;
    }
    return ret;
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
    bool ret = true;
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
        ret = ApplyToMatrix(errors, logprefix, *area.wind.series, col, tsGenCountWind) && ret;
    }
    return ret;
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
    bool ret = true;
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
        ret = ApplyToMatrix(errors, logprefix, *area.solar.series, col, tsGenCountSolar) && ret;
    }
    return ret;
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
    bool ret = true;
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
        ret = ApplyToMatrix(errors, logprefix, *area.hydro.series, col, tsGenCountHydro) && ret;
    }
    return ret;
}

} // namespace Antares
