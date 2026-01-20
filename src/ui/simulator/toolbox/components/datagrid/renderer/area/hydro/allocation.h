// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_HYDRO_ALLOCATION_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_HYDRO_ALLOCATION_H__

#include <antares/date/date.h>
#include <antares/study/parts/wind/prepro.h>
#include "../../../renderer.h"

namespace Antares::Component::Datagrid::Renderer
{
class HydroAllocation final: public virtual Renderer::IRenderer
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    HydroAllocation();
    //! Destructor
    virtual ~HydroAllocation();
    //@}

    virtual int width() const;
    virtual int height() const;

    virtual wxString columnCaption(int colIndx) const;

    virtual wxString rowCaption(int rowIndx) const;

    virtual wxString cellValue(int x, int y) const;

    virtual double cellNumericValue(int x, int y) const;

    virtual bool cellValue(int, int, const Yuni::String&);

    virtual void resetColors(int, int, wxColour&, wxColour&) const
    { /*Do nothing*/
    }

    virtual IRenderer::CellStyle cellStyle(int col, int row) const;

    virtual bool valid() const;

    virtual uint maxWidthResize() const
    {
        return 0;
    }

    virtual uint maxHeightResize() const
    {
        return 0;
    }

}; // class HydroAllocation

} // namespace Antares::Component::Datagrid::Renderer

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_HYDRO_ALLOCATION_H__
