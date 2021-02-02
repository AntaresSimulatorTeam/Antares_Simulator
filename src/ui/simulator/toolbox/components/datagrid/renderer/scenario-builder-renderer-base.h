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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_SCENARIO_BUILDER_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_SCENARIO_BUILDER_H__

#include "../renderer.h"
#include <antares/study/scenario-builder/rules.h>

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
    //! \name Constructors & Destructor
    //@{
    /*!
    ** \brief Constructor with a given timeseries
    */
    ScBuilderRendererBase();
    /*!
    ** \brief Constructor for thermal
    */
    //! Destructor
    virtual ~ScBuilderRendererBase();
    //@}

    virtual int width() const;
    virtual int height() const;

    virtual wxString columnCaption(int colIndx) const;

    virtual wxString rowCaption(int rowIndx) const;

    virtual wxString cellValue(int x, int y) const;

    virtual double cellNumericValue(int x, int y) const = 0;

    virtual bool cellValue(int x, int y, const Yuni::String& value) = 0;

    virtual void resetColors(int, int, wxColour&, wxColour&) const
    {
        // Do nothing
    }

    virtual bool valid() const;

    virtual uint maxWidthResize() const
    {
        return 0;
    }
    virtual IRenderer::CellStyle cellStyle(int col, int row) const;

    void control(wxWindow* control)
    {
        pControl = control;
    }

public:
    //! Event: A rules set has been changed
    void onRulesChanged(Data::ScenarioBuilder::Rules::Ptr rules);

protected:
    void onAreaChanged(Data::Area* area);
    virtual void onStudyClosed();

protected:
    wxWindow* pControl;
    Data::Area* pArea;
    Data::ScenarioBuilder::Rules::Ptr pRules;

}; // class ScBuilderRendererBase

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_SCENARIO_BUILDER_H__
