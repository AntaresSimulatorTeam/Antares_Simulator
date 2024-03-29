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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_SCENARIO_BUILDER_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_SCENARIO_BUILDER_H__

#include "../renderer.h"
#include <antares/study/scenario-builder/rules.h>
#include "../../../../toolbox/input/area.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
class ScBuilderRendererBase : public IRenderer
{
public:
    ScBuilderRendererBase() = default;
    virtual ~ScBuilderRendererBase();

    int width() const override;
    wxString columnCaption(int colIndx) const override;
    wxString cellValue(int x, int y) const override;

    void resetColors(int, int, wxColour&, wxColour&) const override
    {
        // Do nothing
    }

    uint maxWidthResize() const override
    {
        return 0;
    }
    IRenderer::CellStyle cellStyle(int col, int row) const override;

    void control(wxWindow* gridPanel)
    {
        pGridPanel = gridPanel;
    }

public:
    //! Event: A rules set has been changed
    void onRulesChanged(Data::ScenarioBuilder::Rules::Ptr rules);

protected:
    virtual void onStudyClosed();
    wxWindow* gridPanel()
    {
        return pGridPanel;
    }

protected:
    Data::ScenarioBuilder::Rules::Ptr pRules;

private:
    wxWindow* pGridPanel = nullptr;

}; // class ScBuilderRendererBase

// -------------------------------------------------------------------
// Class ScBuilderRendererAreasAsRows
//      Renderer for a scenario builder grid of which lines are
//      names of area.
// ------------------------------------------------------------------
class ScBuilderRendererAreasAsRows : public ScBuilderRendererBase
{
public:
    ScBuilderRendererAreasAsRows() = default;
    ~ScBuilderRendererAreasAsRows() override = default;

    int height() const override;
    wxString rowCaption(int rowIndx) const override;

    bool valid() const override;
};

// -------------------------------------------------------------------
// Class ScBuilderRendererForAreaSelector
//      Renderer for a scenario builder grid of which lines depend
//      on the selected area.
//      Example : grid lines are clusters of an area.
// ------------------------------------------------------------------
class ScBuilderRendererForAreaSelector : public ScBuilderRendererBase
{
public:
    explicit ScBuilderRendererForAreaSelector(const Toolbox::InputSelector::Area* notifier);
    ~ScBuilderRendererForAreaSelector() override = default;

    bool valid() const override;

protected:
    void onAreaChanged(Data::Area* area);
    void onStudyClosed() override;
    Data::Area* selectedArea() const
    {
        return pArea;
    }

private:
    Data::Area* pArea = nullptr;
};

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_SCENARIO_BUILDER_H__
