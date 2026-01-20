// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_MISC_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_MISC_H__

#include "../area.h"
#include "../matrix.h"

namespace Antares::Component::Datagrid::Renderer
{
class Misc: public Renderer::Matrix<>, public Renderer::ARendererArea
{
public:
    //! \name Constructor & Destructor
    //@{

    /*!
    ** \brief Constructor
    */
    Misc(wxWindow* control, Toolbox::InputSelector::Area* notifier);

    //! Destructor
    virtual ~Misc();

    //@}

    virtual int width() const
    {
        return Renderer::Matrix<>::width() + 1;
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

    virtual wxString cellValue(int x, int y) const;

    virtual double cellNumericValue(int x, int y) const;

    virtual bool cellValue(int x, int y, const Yuni::String& value)
    {
        return Renderer::Matrix<>::cellValue(x, y, value);
    }

    virtual void resetColors(int, int, wxColour&, wxColour&) const
    { /*Do nothing*/
    }

    virtual IRenderer::CellStyle cellStyle(int col, int row) const;

    virtual wxColour horizontalBorderColor(int x, int y) const;
    virtual wxColour verticalBorderColor(int x, int y) const;

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

}; // class Misc

} // namespace Antares::Component::Datagrid::Renderer

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_MISC_H__
