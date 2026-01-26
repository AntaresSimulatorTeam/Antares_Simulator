// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_CONVERTION_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_CONVERTION_H__

#include "../area.h"
#include "../matrix.h"

namespace Antares::Component::Datagrid::Renderer
{
template<enum Data::TimeSeriesType T>
class XCastConversion: public Renderer::Matrix<float>, public Renderer::ARendererArea
{
public:
    //! Ancestor
    using MatrixAncestorType = Renderer::Matrix<float>;

public:
    //! \name Constructor & Destructor
    //@{

    /*!
    ** \brief Constructor
    */
    XCastConversion(wxWindow* control, Toolbox::InputSelector::Area* notifier);

    //! Destructor
    virtual ~XCastConversion();

    //@}

    virtual int width() const
    {
        return MatrixAncestorType::width() - 2;
    }

    virtual int height() const
    {
        return MatrixAncestorType::height();
    }

    virtual wxString columnCaption(int colIndx) const;

    virtual wxString rowCaption(int rowIndx) const;

    virtual wxString cellValue(int x, int y) const;

    virtual double cellNumericValue(int x, int y) const;

    virtual bool cellValue(int x, int y, const Yuni::String& value);

    virtual void resetColors(int, int, wxColour&, wxColour&) const
    { /*Do nothing*/
    }

    virtual IRenderer::CellStyle cellStyle(int col, int row) const;

    virtual uint maxWidthResize() const
    {
        return Data::XCast::conversionMaxPoints;
    }

    virtual bool valid() const
    {
        return MatrixAncestorType::valid();
    }

    virtual bool onMatrixResize(uint oldX, uint oldY, uint& newX, uint& newY);
    virtual void onMatrixLoad();

protected:
    virtual void internalAreaChanged(Data::Area* area);
    //! Event: the study has been closed
    virtual void onStudyClosed() override;
    //! Event: the study has been loaded
    virtual void onStudyLoaded() override;

}; // class XCastConversion

} // namespace Antares::Component::Datagrid::Renderer

#include "xcast-conversion.hxx"

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_CONVERTION_H__
