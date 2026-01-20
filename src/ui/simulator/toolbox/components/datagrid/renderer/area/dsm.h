// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_DSM_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_DSM_H__

#include "../area.h"
#include "../matrix.h"
#include <antares/date/date.h>
#include <antares/study/parts/wind/prepro.h>

namespace Antares::Component::Datagrid::Renderer
{
class DSM final: public Renderer::Matrix<>, public Renderer::ARendererArea
{
public:
    //! \name Destructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    DSM(wxWindow* control, Toolbox::InputSelector::Area* notifier);

    //! Destructor
    virtual ~DSM();

    //@}

    virtual int width() const
    {
        return Renderer::Matrix<>::width();
    }

    virtual int height() const
    {
        return Renderer::Matrix<>::height();
    }

    virtual wxString columnCaption(int colIndx) const;

    virtual wxString rowCaption(int rowIndx) const
    {
        return Renderer::Matrix<>::rowCaption(rowIndx);
    }

    virtual wxString cellValue(int x, int y) const
    {
        return Renderer::Matrix<>::cellValue(x, y);
    }

    virtual double cellNumericValue(int x, int y) const
    {
        return Renderer::Matrix<>::cellNumericValue(x, y);
    }

    virtual bool cellValue(int x, int y, const Yuni::String& value)
    {
        return Renderer::Matrix<>::cellValue(x, y, value);
    }

    virtual void resetColors(int, int, wxColour&, wxColour&) const
    { /*Do nothing*/
    }

    virtual IRenderer::CellStyle cellStyle(int col, int row) const;

    virtual wxColour horizontalBorderColor(int x, int y) const;

    virtual uint maxWidthResize() const
    {
        return 0;
    }

    virtual bool valid() const
    {
        return Renderer::Matrix<>::valid();
    }

    virtual Date::Precision precision()
    {
        return Date::hourly;
    }

protected:
    virtual void internalAreaChanged(Antares::Data::Area* area);

    //! Event: the study has been closed
    virtual void onStudyClosed() override;
    //! Event: the study has been loaded
    virtual void onStudyLoaded() override;

private:
    //! The current area
    Antares::Data::Area* pArea;

}; // class DSM

} // namespace Antares::Component::Datagrid::Renderer

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_DSM_H__
