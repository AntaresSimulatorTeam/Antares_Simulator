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

#include "scenario-builder-renderer-base.h"
#include "../../refresh.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
// ==================================
// Scenario builder base class
// ==================================

ScBuilderRendererBase::~ScBuilderRendererBase()
{
    destroyBoundEvents();
}

void ScBuilderRendererBase::onRulesChanged(Data::ScenarioBuilder::Rules::Ptr rules)
{
    if (pRules != rules)
    {
        pRules = rules;
        invalidate = true;
        onRefresh();
        if (gridPanel())
            RefreshAllControls(gridPanel());
    }
}

int ScBuilderRendererBase::width() const
{
    return (!study || !pRules) ? 0 : (int)study->parameters.nbYears;
}

wxString ScBuilderRendererBase::columnCaption(int x) const
{
    return wxString(wxT("year ")) << (1 + x);
}

wxString ScBuilderRendererBase::cellValue(int x, int y) const
{
    const double d = cellNumericValue(x, y);
    return (Math::Zero(d)) ? wxString() << wxT("rand") : wxString() << (uint)d;
}

static IRenderer::CellStyle alternateEnabledDisabled(int rowIndex, bool enabled)
{
    if (enabled)
    {
        if (rowIndex % 2 == 0)
            return IRenderer::cellStyleDefaultCenter;
        else
            return IRenderer::cellStyleDefaultCenterAlternate;
    }
    else
    {
        if (rowIndex % 2 == 0)
            return IRenderer::cellStyleDefaultCenterDisabled;
        else
            return IRenderer::cellStyleDefaultCenterAlternateDisabled;
    }
}

IRenderer::CellStyle ScBuilderRendererBase::cellStyle(int x, int y) const
{
    if (Math::Zero(cellNumericValue(x, y)))
        return alternateEnabledDisabled(y, false);

    bool valid = (!(!study) && !(!pRules));
    if (valid)
    {
        auto& parameters = study->parameters;
        if (parameters.userPlaylist)
            valid = parameters.yearsFilter[x];
    }
    return alternateEnabledDisabled(y, valid);
}

void ScBuilderRendererBase::onStudyClosed()
{
    pRules = nullptr;
    IRenderer::onStudyClosed();
}

// ========================================
// Class ScBuilderRendererAreasAsRows
// ========================================

int ScBuilderRendererAreasAsRows::height() const
{
    if (!(!study) && !(!pRules))
        return (int)pRules->areaCount();
    return 0;
}

wxString ScBuilderRendererAreasAsRows::rowCaption(int rowIndx) const
{
    if (!(!study) && !(!pRules))
    {
        if ((uint)rowIndx < study->areas.size())
            return wxString() << wxT(" ") << wxStringFromUTF8(study->areas.byIndex[rowIndx]->name)
                              << wxT("  ");
    }
    return wxEmptyString;
}

bool ScBuilderRendererAreasAsRows::valid() const
{
    return !(!study) && pRules && study->areas.size() != 0 && !(!pRules);
}

// ========================================
// Class ScBuilderRendererForAreaSelector
// ========================================
ScBuilderRendererForAreaSelector::ScBuilderRendererForAreaSelector(
  const Toolbox::InputSelector::Area* notifier)
{
    if (notifier)
    {
        // Event: The current selected area
        Toolbox::InputSelector::Area::onAreaChanged.connect(
          this, &ScBuilderRendererForAreaSelector::onAreaChanged);
    }
}

void ScBuilderRendererForAreaSelector::onStudyClosed()
{
    pArea = nullptr;
    ScBuilderRendererBase::onStudyClosed();
}

void ScBuilderRendererForAreaSelector::onAreaChanged(Data::Area* area)
{
    if (area != pArea)
    {
        pArea = area;
        onRefresh();
        if (gridPanel())
            gridPanel()->Refresh();
    }
}

bool ScBuilderRendererForAreaSelector::valid() const
{
    return !(!study) && pRules && !study->areas.empty() && !(!pRules) && pArea;
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
