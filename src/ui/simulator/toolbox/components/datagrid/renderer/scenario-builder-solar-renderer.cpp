// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "scenario-builder-solar-renderer.h"
#include "antares/study/scenario-builder/scBuilderUtils.h"

using namespace Yuni;
using namespace Antares::Data::ScenarioBuilder;

namespace Antares::Component::Datagrid::Renderer
{
bool solarScBuilderRenderer::cellValue(int x, int y, const Yuni::String& value)
{
    if (!(!study) && !(!pRules) && (uint)x < study->parameters.nbYears)
    {
        if ((uint)y < study->areas.size())
        {
            assert((uint)y < pRules->solar.width());
            assert((uint)x < pRules->solar.height());
            uint val = fromStringToTSnumber(value);
            pRules->solar.set_value(x, y, val);
            return true;
        }
    }
    return false;
}

double solarScBuilderRenderer::cellNumericValue(int x, int y) const
{
    if (!(!study) && !(!pRules) && (uint)x < study->parameters.nbYears)
    {
        if ((uint)y < study->areas.size())
        {
            assert((uint)y < pRules->solar.width());
            assert((uint)x < pRules->solar.height());
            return pRules->solar.get_value(x, y);
        }
    }
    return 0.;
}

} // namespace Antares::Component::Datagrid::Renderer
