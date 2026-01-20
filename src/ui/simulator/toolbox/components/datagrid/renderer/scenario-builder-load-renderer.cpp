// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "scenario-builder-load-renderer.h"
#include "antares/study/scenario-builder/scBuilderUtils.h"

using namespace Yuni;
using namespace Antares::Data::ScenarioBuilder;

namespace Antares::Component::Datagrid::Renderer
{
bool loadScBuilderRenderer::cellValue(int x, int y, const Yuni::String& value)
{
    if (!(!study) && !(!pRules) && (uint)x < study->parameters.nbYears)
    {
        if ((uint)y < study->areas.size())
        {
            assert((uint)y < pRules->load.width());
            assert((uint)x < pRules->load.height());
            uint val = fromStringToTSnumber(value);
            pRules->load.set_value(x, y, val);
            return true;
        }
    }
    return false;
}

double loadScBuilderRenderer::cellNumericValue(int x, int y) const
{
    if (!(!study) && !(!pRules) && (uint)x < study->parameters.nbYears)
    {
        if ((uint)y < study->areas.size())
        {
            assert((uint)y < pRules->load.width());
            assert((uint)x < pRules->load.height());
            return pRules->load.get_value(x, y);
        }
    }
    return 0.;
}

} // namespace Antares::Component::Datagrid::Renderer
