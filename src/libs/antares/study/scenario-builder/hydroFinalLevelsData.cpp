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

#include <sstream>
#include <iomanip>
#include "hydroFinalLevelsData.h"
#include "scBuilderUtils.h"

namespace Antares
{
namespace Data
{
namespace ScenarioBuilder
{
bool hydroFinalLevelsData::reset(const Study& study)
{
    const uint nbYears = study.parameters.nbYears;

    pHydroFinalLevelsRules.reset(study.areas.size(), nbYears);
    pHydroFinalLevelsRules.fill(std::nan(""));

    return true;
}

void hydroFinalLevelsData::saveToINIFile(const Study& study, Yuni::IO::File::Stream& file) const
{
    // Prefix
    CString<512, false> prefix;
    prefix += "hfl,";

    // Turning values into strings (precision 4)
    std::ostringstream value_into_string;
    value_into_string << std::setprecision(4);

    // Foreach year
    assert(pHydroFinalLevelsRules.width == study.areas.size());
    for (uint index = 0; index != pHydroFinalLevelsRules.width; ++index)
    {
        // alias to the current column
        const MatrixType::ColumnType& col = pHydroFinalLevelsRules[index];
        // Foreach area...
        for (uint y = 0; y != pHydroFinalLevelsRules.height; ++y)
        {
            const MatrixType::Type value = col[y];
            // Equals to zero means 'auto', which is the default mode
            if (std::isnan(value))
                continue;
            assert(index < study.areas.size());
            value_into_string << value;
            file << prefix << study.areas.byIndex[index]->id << ',' << y << " = "
                 << value_into_string.str() << '\n';
            value_into_string.str(string()); // Clearing converter
        }
    }
}

void hydroFinalLevelsData::set_value(uint x, uint y, double value)
{
    pHydroFinalLevelsRules.entry[y][x] = value;
}

bool hydroFinalLevelsData::apply(Study& study)
{
    study.scenarioFinalHydroLevels.copyFrom(pHydroFinalLevelsRules);
    return true;
}

} // namespace ScenarioBuilder
} // namespace Data
} // namespace Antares
