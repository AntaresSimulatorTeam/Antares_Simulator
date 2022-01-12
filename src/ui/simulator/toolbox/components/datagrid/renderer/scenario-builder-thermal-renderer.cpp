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
    if (!(!study) && !(!pRules) && selectedArea() && (uint)rowIndx < selectedArea()->thermal.list.size())
    {
        return wxString() << wxT(" ")
                            << wxStringFromUTF8(selectedArea()->thermal.list.byIndex[rowIndx]->name())
                            << wxT("  ");
    }
    return wxEmptyString;
}

bool thermalScBuilderRenderer::cellValue(int x, int y, const String& value)
{
    if (!(!study) && !(!pRules) && (uint)x < study->parameters.nbYears && selectedArea() && 
        (uint)y < selectedArea()->thermal.list.size())
    {
        assert(selectedArea()->index < pRules->areaCount());
        assert((uint)y < pRules->thermal[selectedArea()->index].width());
        assert((uint)x < pRules->thermal[selectedArea()->index].height());
        uint val = fromStringToTSnumber(value);
        pRules->thermal[selectedArea()->index].set(selectedArea()->thermal.list.byIndex[y], x, val);
        return true;
    }
    return false;
}

double thermalScBuilderRenderer::cellNumericValue(int x, int y) const
{
    if (!(!study) && !(!pRules) && (uint)x < study->parameters.nbYears && selectedArea() && 
        (uint)y < selectedArea()->thermal.list.size())
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
