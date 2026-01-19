// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_LAYERS_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_LAYERS_H__

#include "../renderer.h"
#include "../../../../application/study.h"
#include "../../../../toolbox/input/area.h"
#include <antares/study/scenario-builder/rules.h>

namespace Antares::Component::Datagrid::Renderer
{
/*Per area layers ui grid renderer*/
class LayersUI: public IRenderer
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
class LayersVisibility: public IRenderer
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

} // namespace Antares::Component::Datagrid::Renderer

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_LAYERS_H__
