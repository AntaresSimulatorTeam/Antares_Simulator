// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_NODAL_OPTIMIZATION_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_NODAL_OPTIMIZATION_H__

#include "../../gridhelper.h"
#include "../../renderer.h"
#include <antares/study/study.h>
#include <yuni/core/event.h>

namespace Antares::Component::Datagrid::Renderer
{
class NodalOptimization: public Renderer::IRenderer
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    NodalOptimization(wxWindow* parent);

    //! Destructor
    virtual ~NodalOptimization();
    //@}

    virtual int width() const;

    virtual int height() const;

    virtual wxString columnCaption(int colIndx) const;

    virtual wxString rowCaption(int rowIndx) const;

    virtual wxString cellValue(int, int) const;

    virtual double cellNumericValue(int, int) const;

    virtual bool cellValue(int x, int y, const Yuni::String& value);

    virtual void resetColors(int, int, wxColour&, wxColour&) const
    { /*Do nothing*/
    }

    virtual void applyLayerFiltering(size_t layerID, VGridHelper* gridHelper);

    virtual IRenderer::CellStyle cellStyle(int col, int row) const;

    virtual bool valid() const;

protected:
    virtual void onUpdate()
    {
    }

    void onAreaRenamed(Data::Area* area);
    void onNodalOptimizationExternalChanged();

private:
    wxWindow* pControl;

}; // class NodalOptimization

} // namespace Antares::Component::Datagrid::Renderer

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_NODAL_OPTIMIZATION_H__
