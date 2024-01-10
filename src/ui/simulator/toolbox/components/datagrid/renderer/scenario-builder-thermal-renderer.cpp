/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
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

#include "scenario-builder-thermal-renderer.h"
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
int thermalScBuilderRenderer::height() const
{
    if (!(!study) && !(!pRules) && selectedArea())
    {
        return (int)selectedArea()->thermal.list.size();
    }
    return 0;
}

wxString thermalScBuilderRenderer::rowCaption(int rowIndx) const
{
    if (!(!study) && !(!pRules) && selectedArea()
        && (uint)rowIndx < selectedArea()->thermal.list.size())
    {
        return wxString() << wxT(" ")
                          << wxStringFromUTF8(selectedArea()->thermal.list[rowIndx]->name())
                          << wxT("  ");
    }
    return wxEmptyString;
}

bool thermalScBuilderRenderer::cellValue(int x, int y, const String& value)
{
    if (!(!study) && !(!pRules) && (uint)x < study->parameters.nbYears && selectedArea()
        && (uint)y < selectedArea()->thermal.list.size())
    {
        assert(selectedArea()->index < pRules->areaCount());
        assert((uint)y < pRules->thermal[selectedArea()->index].width());
        assert((uint)x < pRules->thermal[selectedArea()->index].height());
        uint val = fromStringToTSnumber(value);
        pRules->thermal[selectedArea()->index].setTSnumber(selectedArea()->thermal.list[y].get(), x, val);
        return true;
    }
    return false;
}

double thermalScBuilderRenderer::cellNumericValue(int x, int y) const
{
    if (!(!study) && !(!pRules) && (uint)x < study->parameters.nbYears && selectedArea()
        && (uint)y < selectedArea()->thermal.list.size())
    {
        assert((uint)y < pRules->thermal[selectedArea()->index].width());
        assert((uint)x < pRules->thermal[selectedArea()->index].height());
        return pRules->thermal[selectedArea()->index].get_value(x, y);
    }
    return 0.;
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
