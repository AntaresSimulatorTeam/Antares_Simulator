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

} // namespace Antares
