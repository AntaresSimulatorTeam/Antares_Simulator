// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "scenario-builder-wind-renderer.h"
#include "antares/study/scenario-builder/scBuilderUtils.h"

using namespace Yuni;
using namespace Antares::Data::ScenarioBuilder;

namespace Antares::Component::Datagrid::Renderer
{
bool windScBuilderRenderer::cellValue(int x, int y, const Yuni::String& value)
{
    if (!(!study) && !(!pRules) && (uint)x < study->parameters.nbYears)
    {
        if ((uint)y < study->areas.size())
        {
            assert((uint)y < pRules->wind.width());
            assert((uint)x < pRules->wind.height());
            uint val = fromStringToTSnumber(value);
            pRules->wind.set_value(x, y, val);
            return true;
        }
    }
    return false;
}

double windScBuilderRenderer::cellNumericValue(int x, int y) const
{
    if (!(!study) && !(!pRules) && (uint)x < study->parameters.nbYears)
    {
        if ((uint)y < study->areas.size())
        {
            assert((uint)y < pRules->wind.width());
            assert((uint)x < pRules->wind.height());
            return pRules->wind.get_value(x, y);
        }
    }
    return 0.;
}

} // namespace Antares::Component::Datagrid::Renderer
