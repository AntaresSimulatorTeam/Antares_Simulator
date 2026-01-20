// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_BINDINGCONSTRAINT_DATA_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_BINDINGCONSTRAINT_DATA_H__

#include "../../gridhelper.h"
#include "../../renderer.h"
#include <yuni/core/event.h>
#include "../../../../../application/study.h"

namespace Antares::Component::Datagrid::Renderer::BindingConstraint
{
class Data final: public virtual IRenderer
{
public:
    Data(wxWindow* parent, const Antares::Data::BindingConstraint::Operator op);
    virtual ~Data();

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

    virtual void applyLayerFiltering(size_t layerID, VGridHelper* gridHelper);

    virtual bool valid() const
    {
        return (width() > 0);
    }

    virtual IRenderer::CellStyle cellStyle(int x, int y) const;

    virtual wxColour horizontalBorderColor(int x, int y) const;

    void bindingConstraintTypeChanged(Antares::Data::BindingConstraint::Type type);

    void onStudyChanged(Antares::Data::Study&);

    virtual Date::Precision precision();

protected:
    const Antares::Data::BindingConstraint::Operator pOperator;
    wxWindow* pControl;
    wxString pZero;
    Antares::Data::BindingConstraint::Type pType;
    Antares::Data::BindingConstraint::Column pColumn;

}; // class Data

} // namespace Antares::Component::Datagrid::Renderer::BindingConstraint

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_BINDINGCONSTRAINT_DATA_H__
