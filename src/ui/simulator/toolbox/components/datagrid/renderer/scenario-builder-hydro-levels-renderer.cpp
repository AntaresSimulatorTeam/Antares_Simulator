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

#include "scenario-builder-hydro-levels-renderer.h"
#include "antares/study/scenario-builder/scBuilderUtils.h"

using namespace Antares::Data::ScenarioBuilder;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
wxString hydroLevelsScBuilderRenderer::cellValue(int x, int y) const
{
    const double d = cellNumericValue(x, y);
    return (std::isnan(d)) ? wxString() << wxT("rand") : wxString() << fromHydroLevelToString(d);
}

bool hydroLevelsScBuilderRenderer::cellValue(int x, int y, const Yuni::String& value)
{
    if (!(!study) && !(!pRules) && (uint)x < study->parameters.nbYears
        && (uint)y < study->areas.size())
    {
        assert((uint)y < pRules->hydroInitialLevels.width());
        assert((uint)x < pRules->hydroInitialLevels.height());
        double val = fromStringToHydroLevel(value, 100.) / 100.;
        pRules->hydroInitialLevels.set_value(x, y, val);
        return true;
    }
    return false;
}

double hydroLevelsScBuilderRenderer::cellNumericValue(int x, int y) const
{
    if (!(!study) && !(!pRules) && (uint)x < study->parameters.nbYears
        && (uint)y < study->areas.size())
    {
        assert((uint)y < pRules->hydroInitialLevels.width());
        assert((uint)x < pRules->hydroInitialLevels.height());
        return pRules->hydroInitialLevels.get_value(x, y) * 100.;
    }
    return 0.;
}

IRenderer::CellStyle hydroLevelsScBuilderRenderer::cellStyle(int x, int y) const
{
    bool valid = (!(!study) && !(!pRules) && std::isnan(cellNumericValue(x, y)));
    return valid ? cellStyleDefaultCenterDisabled : cellStyleDefaultCenter;
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
