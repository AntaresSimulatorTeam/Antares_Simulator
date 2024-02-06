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

#include "renderer.h"
#include "../../../application/study.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
const wxChar* IRenderer::CellStyleToCSSClass(CellStyle s)
{
    switch (s)
    {
    case cellStyleDefault:
        return wxT("df");
    case cellStyleDefaultDisabled:
        return wxT("df_");
    case cellStyleDefaultAlternate:
        return wxT("da");
    case cellStyleDefaultAlternateDisabled:
        return wxT("da_");
    case cellStyleDefaultCenter:
        return wxT("cdf");
    case cellStyleDefaultCenterDisabled:
        return wxT("cdf_");
    case cellStyleDefaultCenterAlternate:
        return wxT("cda");
    case cellStyleDefaultCenterAlternateDisabled:
        return wxT("cda_");
    case cellStyleWarning:
        return wxT("wrn");
    case cellStyleError:
        return wxT("sce");
    case cellStyleDisabled:
        return wxT("ds");
    case cellStyleAverage:
        return wxT("avg");
    case cellStyleSum:
        return wxT("sum");
    case cellStyleMinMax:
        return wxT("mim");
    case cellStyleConstraintEnabled:
        return wxT("cston");
    case cellStyleConstraintDisabled:
        return wxT("cstoff");
    case cellStyleConstraintWeight:
        return wxT("cstwg");
    case cellStyleConstraintNoWeight:
        return wxT("cstwn");
    case cellStyleConstraintWeightCount:
        return wxT("cstwc");
    case cellStyleConstraintOperator:
        return wxT("cstop");
    case cellStyleConstraintType:
        return wxT("cstp");

    default:
        return wxT("ukn");
    }
}

void IRenderer::CellStyleToCSSClass(CellStyle s, Yuni::String& str)
{
    switch (s)
    {
    case cellStyleDefault:
        str += "df";
        break;
    case cellStyleDefaultDisabled:
        str += "df_";
        break;
    case cellStyleDefaultAlternate:
        str += "da";
        break;
    case cellStyleDefaultAlternateDisabled:
        str += "da_";
        break;
    case cellStyleDefaultCenter:
        str += "cdf";
        break;
    case cellStyleDefaultCenterDisabled:
        str += "cdf_";
        break;
    case cellStyleDefaultCenterAlternate:
        str += "cda";
        break;
    case cellStyleDefaultCenterAlternateDisabled:
        str += "cda_";
        break;
    case cellStyleWarning:
        str += "wrn";
        break;
    case cellStyleError:
        str += "sce";
        break;
    case cellStyleDisabled:
        str += "ds";
        break;
    case cellStyleAverage:
        str += "avg";
        break;
    case cellStyleSum:
        str += "sum";
        break;
    case cellStyleMinMax:
        str += "mim";
        break;
    case cellStyleConstraintEnabled:
        str += "cston";
        break;
    case cellStyleConstraintDisabled:
        str += "cstoff";
        break;
    case cellStyleConstraintWeight:
        str += "cstwg";
        break;
    case cellStyleConstraintNoWeight:
        str += "cstwn";
        break;
    case cellStyleConstraintWeightCount:
        str += "cstwc";
        break;
    case cellStyleConstraintOperator:
        str += "cstop";
        break;
    case cellStyleConstraintType:
        str += "cstp";
        break;
    default:
        str += "ukn";
    }
}

void IRenderer::appendCellValue(int x, int y, Yuni::String& s) const
{
    String t;
    wxStringToString(this->cellValue(x, y), t);
    s += t;
}

IRenderer::IRenderer() : dataGridPrecision(Date::stepNone), invalidate(false)
{
    // Event: The study has been closed
    OnStudyClosed.connect(this, &IRenderer::onStudyClosed);
    OnStudyLoaded.connect(this, &IRenderer::onStudyLoaded);
}

IRenderer::~IRenderer()
{
    destroyBoundEvents();
    onRefresh.clear();
}

void IRenderer::onStudyLoaded()
{
    study = GetCurrentStudy();
}

void IRenderer::onStudyClosed()
{
    study = nullptr; // unlinking the pointer
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
