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

#include "antares/study/scenario-builder/TSnumberData.h"

#include "antares/study/scenario-builder/applyToMatrix.hxx"
#include "antares/study/scenario-builder/scBuilderUtils.h"

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
            {
                continue;
            }
            assert(index < study.areas.size());
            file << prefix << study.areas.byIndex[index]->id << ',' << y << " = " << value << '\n';
        }
    }
}

void TSNumberData::setTSnumber(uint areaindex, uint year, uint value)
{
    assert(areaindex < pTSNumberRules.width);
    if (year < pTSNumberRules.height)
    {
        pTSNumberRules[areaindex][year] = value;
    }
}

void TSNumberData::set_value(uint x, uint y, uint value)
{
    pTSNumberRules.entry[y][x] = value;
}

} // namespace Antares::Data::ScenarioBuilder
