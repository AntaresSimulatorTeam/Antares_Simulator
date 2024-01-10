/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
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

#include "scenario-builder-load-renderer.h"
#include "antares/study/scenario-builder/scBuilderUtils.h"

using namespace Yuni;
using namespace Antares::Data::ScenarioBuilder;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
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

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
