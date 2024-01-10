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

#include "select-variables.h"
#include <yuni/core/math.h>

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
SelectVariables::SelectVariables() : pControl(nullptr)
{
}

SelectVariables::~SelectVariables()
{
}

bool SelectVariables::valid() const
{
    if (!study)
        std::cout << "not valid ! \n";
    return !(!study);
}

int SelectVariables::height() const
{
    return (not study) ? 0 : (int)study->parameters.variablesPrintInfo.size();
}

wxString SelectVariables::columnCaption(int) const
{
    return wxT("   Status   ");
}

wxString SelectVariables::rowCaption(int rowIndx) const
{
    return wxString(wxT(" ")) << study->parameters.variablesPrintInfo.name_of(rowIndx) << wxT("  ");
}

bool SelectVariables::cellValue(int, int var, const Yuni::String& value)
{
    if (!(!study) && (uint)var < study->parameters.variablesPrintInfo.size())
    {
        String s = value;
        s.trim();
        s.toLower();
        bool v = s.to<bool>() || s == "active" || s == "enabled";
        study->parameters.variablesPrintInfo.setPrintStatus(var, v);
        onTriggerUpdate();
        Dispatcher::GUI::Refresh(pControl);
        return true;
    }
    return false;
}

double SelectVariables::cellNumericValue(int, int var) const
{
    if (!(!study) && (uint)var < study->parameters.variablesPrintInfo.size())
    {
        return study->parameters.variablesPrintInfo[var].isPrinted();
    }
    return 0.;
}

wxString SelectVariables::cellValue(int, int var) const
{
    if (!(!study) && static_cast<uint>(var) < study->parameters.variablesPrintInfo.size())
    {
        return study->parameters.variablesPrintInfo[var].isPrinted() ? wxT("Active") : wxT("skip");
    }
    return wxEmptyString;
}

IRenderer::CellStyle SelectVariables::cellStyle(int, int var) const
{
    if (!(!study) && (uint)var < study->parameters.variablesPrintInfo.size())
    {
        return !study->parameters.variablesPrintInfo[var].isPrinted()
                 ? IRenderer::cellStyleConstraintNoWeight
                 : IRenderer::cellStyleConstraintWeight;
    }
    return IRenderer::cellStyleConstraintNoWeight;
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
