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
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "scenario-builder-renewable-renderer.h"
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
int renewableScBuilderRenderer::height() const
{
    if (!(!study) && !(!pRules) && selectedArea())
    {
        return (int)selectedArea()->renewable.list.allClustersCount();
    }
    return 0;
}

wxString renewableScBuilderRenderer::rowCaption(int rowIndx) const
{
    if (!(!study) && !(!pRules) && selectedArea()
        && (uint)rowIndx < selectedArea()->renewable.list.allClustersCount())
    {
        return wxString() << wxT(
                 " ") << wxStringFromUTF8(selectedArea()->renewable.list[rowIndx]->name())
                          << wxT("  ");
    }
    return wxEmptyString;
}

bool renewableScBuilderRenderer::cellValue(int x, int y, const String& value)
{
    if (!(!study) && !(!pRules) && (uint)x < study->parameters.nbYears && selectedArea()
        && (uint)y < selectedArea()->renewable.list.allClustersCount())
    {
        assert(selectedArea()->index < pRules->areaCount());
        assert((uint)y < pRules->renewable[selectedArea()->index].width());
        assert((uint)x < pRules->renewable[selectedArea()->index].height());
        uint val = fromStringToTSnumber(value);
        pRules->renewable[selectedArea()->index].setTSnumber(
          selectedArea()->renewable.list[y].get(), x, val);
        return true;
    }
    return false;
}

double renewableScBuilderRenderer::cellNumericValue(int x, int y) const
{
    if (!(!study) && !(!pRules) && (uint)x < study->parameters.nbYears && selectedArea()
        && (uint)y < selectedArea()->renewable.list.allClustersCount())
    {
        assert((uint)y < pRules->renewable[selectedArea()->index].width());
        assert((uint)x < pRules->renewable[selectedArea()->index].height());
        return pRules->renewable[selectedArea()->index].get_value(x, y);
    }
    return 0.;
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
