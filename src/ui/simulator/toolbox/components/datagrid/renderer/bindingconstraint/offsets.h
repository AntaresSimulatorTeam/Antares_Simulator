// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_BINDINGCONSTRAINT_OFFSET_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_BINDINGCONSTRAINT_OFFSET_H__

#include "../../gridhelper.h"
#include "../../renderer.h"
#include <yuni/core/event.h>
#include "../../../../../application/study.h"

namespace Antares::Component::Datagrid::Renderer::BindingConstraint
{
class LinkOffsets: public virtual IRenderer
{
public:
    LinkOffsets(wxWindow* parent);
    virtual ~LinkOffsets();

    virtual int width() const;
    virtual int height() const;

    virtual wxString columnCaption(int colIndx) const;

    virtual wxString rowCaption(int rowIndx) const;

    virtual wxString cellValue(int x, int y) const;

    virtual double cellNumericValue(int x, int y) const;

    virtual int cellNumericIntValue(int x, int y) const;

    virtual bool cellValue(int x, int y, const Yuni::String& value);

    virtual void resetColors(int, int, wxColour&, wxColour&) const
    {
        // Do nothing
    }

    virtual bool valid() const;

    virtual IRenderer::CellStyle cellStyle(int x, int y) const;

    virtual wxColour cellBackgroundColor(int x, int y) const;

    wxColour cellTextColor(int x, int y) const;

    virtual void applyLayerFiltering(size_t layerID, VGridHelper* gridHelper);

protected:
    wxWindow* pControl;
    wxString pZero;

}; // class LinkOffsets

class ClusterOffsets: public virtual IRenderer
{
public:
    ClusterOffsets(wxWindow* parent);
    virtual ~ClusterOffsets();

    virtual int width() const;
    virtual int height() const;

    virtual wxString columnCaption(int colIndx) const;

    virtual wxString rowCaption(int rowIndx) const;

    virtual wxString cellValue(int x, int y) const;

    virtual double cellNumericValue(int x, int y) const;

    virtual int cellNumericIntValue(int x, int y) const;

    virtual bool cellValue(int x, int y, const Yuni::String& value);

    virtual void resetColors(int, int, wxColour&, wxColour&) const
    {
        // Do nothing
    }

    virtual bool valid() const;

    virtual IRenderer::CellStyle cellStyle(int x, int y) const;

    virtual wxColour cellBackgroundColor(int x, int y) const;

    wxColour cellTextColor(int x, int y) const;

    virtual void applyLayerFiltering(size_t layerID, VGridHelper* gridHelper);

protected:
    wxWindow* pControl;
    wxString pZero;

}; // class ClusterOffsets

} // namespace Antares::Component::Datagrid::Renderer::BindingConstraint

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_BINDINGCONSTRAINT_OFFSET_H__
