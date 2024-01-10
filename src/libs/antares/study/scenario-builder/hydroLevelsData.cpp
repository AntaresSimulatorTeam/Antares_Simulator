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
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <sstream>
#include <iomanip>
#include "hydroLevelsData.h"
#include "scBuilderUtils.h"

namespace Antares
{
namespace Data
{
namespace ScenarioBuilder
{
bool hydroLevelsData::reset(const Study& study)
{
    const uint nbYears = study.parameters.nbYears;

    pHydroLevelsRules.reset(study.areas.size(), nbYears);
    pHydroLevelsRules.fill(std::nan(""));

    return true;
}

void hydroLevelsData::saveToINIFile(const Study& study, Yuni::IO::File::Stream& file) const
{
    // Prefix
    CString<512, false> prefix;
    prefix += "hl,";

    // Turning values into strings (precision 4)
    std::ostringstream value_into_string;
    value_into_string << std::setprecision(4);

    // Foreach year
    assert(pHydroLevelsRules.width == study.areas.size());
    for (uint index = 0; index != pHydroLevelsRules.width; ++index)
    {
        // alias to the current column
        const MatrixType::ColumnType& col = pHydroLevelsRules[index];
        // Foreach area...
        for (uint y = 0; y != pHydroLevelsRules.height; ++y)
        {
            const MatrixType::Type value = col[y];
            // Equals to zero means 'auto', which is the default mode
            if (std::isnan(value))
                continue;
            assert(index < study.areas.size());
            value_into_string << value;
            file << prefix << study.areas.byIndex[index]->id << ',' << y << " = "
                 << value_into_string.str() << '\n';
            value_into_string.str(std::string()); // Clearing converter
        }
    }
}

void hydroLevelsData::set_value(uint x, uint y, double value)
{
    pHydroLevelsRules.entry[y][x] = value;
}

bool hydroLevelsData::apply(Study& study)
{
    study.scenarioHydroLevels.copyFrom(pHydroLevelsRules);
    return true;
}

} // namespace ScenarioBuilder
} // namespace Data
} // namespace Antares
