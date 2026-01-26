// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_TRANSLATION_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_TRANSLATION_H__

#include "../area.h"
#include "../matrix.h"

namespace Antares::Component::Datagrid::Renderer
{
template<enum Data::TimeSeriesType T>
class XCastTranslation final: public Renderer::Matrix<int32_t>, public Renderer::ARendererArea
{
public:
    //! Ancestor
    using MatrixAncestorType = Renderer::Matrix<int32_t>;

public:
    //! \name Constructor & Destructor
    //@{

    /*!
    ** \brief Constructor
    */
    XCastTranslation(wxWindow* control, Toolbox::InputSelector::Area* notifier);

    //! Destructor
    virtual ~XCastTranslation();

    //@}

    virtual int width() const
    {
        return MatrixAncestorType::width();
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

    virtual wxColour horizontalBorderColor(int x, int y) const;

    virtual uint maxWidthResize() const
    {
        return 0;
    }

    virtual bool valid() const
    {
        return MatrixAncestorType::valid();
    }

    virtual Date::Precision precision()
    {
        return Date::hourly;
    }

protected:
    virtual void internalAreaChanged(Data::Area* area);
    //! Event: the study has been closed
    virtual void onStudyClosed() override;
    //! Event: the study has been loaded
    virtual void onStudyLoaded() override;

}; // class XCastTranslation

} // namespace Antares::Component::Datagrid::Renderer

#include "xcast-translation.hxx"

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_TRANSLATION_H__
