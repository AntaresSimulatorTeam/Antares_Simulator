// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "scenario-builder-hydro-levels-renderer.h"
#include "antares/study/scenario-builder/scBuilderUtils.h"

using namespace Antares::Data::ScenarioBuilder;

namespace Antares::Component::Datagrid::Renderer
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

} // namespace Antares::Component::Datagrid::Renderer
