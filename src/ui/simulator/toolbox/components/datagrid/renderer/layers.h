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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_LAYERS_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_LAYERS_H__

#include "../renderer.h"
#include "../../../../application/study.h"
#include "../../../../toolbox/input/area.h"
#include <antares/study/scenario-builder/rules.h>

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
/*Per area layers ui grid renderer*/
class LayersUI : public IRenderer
{
public:
    /*!
    ** \brief Constructor for Layers from area selection notifier
    */
    LayersUI(Toolbox::InputSelector::Area* notifier);
    //! Destructor
    virtual ~LayersUI();
    //@}

    virtual int width() const;
    virtual int height() const;

    virtual wxString columnCaption(int colIndx) const;

    virtual wxString rowCaption(int rowIndx) const;

    virtual wxString cellValue(int x, int y) const;

    virtual double cellNumericValue(int x, int y) const;

    virtual bool cellValue(int x, int y, const Yuni::String& value);

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

    void control(Antares::Component::Datagrid::Component* control)
    {
        pControl = control;
    }

protected:
    void onAreaChanged(Data::Area* area);
    virtual void onStudyClosed();

protected:
    Antares::Component::Datagrid::Component* pControl;
    Data::Area* pArea;

}; // class LayersUI

/*General layer visibility grid renderer*/
class LayersVisibility : public IRenderer
{
public:
    /*!
    ** \brief Constructor for Layers from area selection notifier
    */
    LayersVisibility();
    //! Destructor
    virtual ~LayersVisibility();
    //@}

    virtual int width() const;
    virtual int height() const;

    virtual wxString columnCaption(int colIndx) const;

    virtual wxString rowCaption(int rowIndx) const;

    virtual wxString cellValue(int x, int y) const;

    virtual double cellNumericValue(int x, int y) const;

    virtual bool cellValue(int x, int y, const Yuni::String& value);

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

    void control(Antares::Component::Datagrid::Component* control)
    {
        pControl = control;
    }

protected:
    virtual void onStudyClosed();

protected:
    Antares::Component::Datagrid::Component* pControl;

}; // class Layers

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_LAYERS_H__
