// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_BINDINGCONSTRAINT_WEIGHT_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_BINDINGCONSTRAINT_WEIGHT_H__

#include "../../gridhelper.h"
#include "../../renderer.h"
#include <yuni/core/event.h>
#include "../../../../../application/study.h"

namespace Antares::Component::Datagrid::Renderer::BindingConstraint
{
class LinkWeights: public virtual IRenderer
{
public:
    LinkWeights(wxWindow* parent);
    virtual ~LinkWeights();

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

    virtual IRenderer::CellStyle cellStyle(int x, int y) const;

    virtual wxColour cellBackgroundColor(int x, int y) const;

    wxColour cellTextColor(int x, int y) const;

    virtual void applyLayerFiltering(size_t layerID, VGridHelper* gridHelper);

protected:
    wxWindow* pControl;
    wxString pZero;

}; // class LinkWeights

class ClusterWeights: public virtual IRenderer
{
public:
    ClusterWeights(wxWindow* parent);
    virtual ~ClusterWeights();

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

    virtual IRenderer::CellStyle cellStyle(int x, int y) const;

    virtual wxColour cellBackgroundColor(int x, int y) const;

    wxColour cellTextColor(int x, int y) const;

    virtual void applyLayerFiltering(size_t layerID, VGridHelper* gridHelper);

protected:
    wxWindow* pControl;
    wxString pZero;

}; // class ClusterWeights

} // namespace Antares::Component::Datagrid::Renderer::BindingConstraint

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_BINDINGCONSTRAINT_WEIGHT_H__
