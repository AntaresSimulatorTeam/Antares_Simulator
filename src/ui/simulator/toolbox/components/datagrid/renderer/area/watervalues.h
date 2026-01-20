// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_WATERVALUES_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_WATERVALUES_H__

#include "../area.h"
#include "../matrix.h"
#include <antares/date/date.h>

namespace Antares::Component::Datagrid::Renderer
{
class WaterValues final: public Renderer::Matrix<double, double, 2>, public Renderer::ARendererArea
{
public:
    using MatrixAncestorType = Renderer::Matrix<double, double, 2>;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    WaterValues(wxWindow* control, Toolbox::InputSelector::Area* notifier);
    //! Destructor
    virtual ~WaterValues();
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

    virtual Date::Precision precision()
    {
        return Date::daily;
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

    // virtual bool circularShiftRowsUntilDate(MonthName month, uint daymonth);

protected:
    virtual void internalAreaChanged(Antares::Data::Area* area);
    //! Event: the study has been closed
    virtual void onStudyClosed() override;
    //! Event: the study has been loaded
    virtual void onStudyLoaded() override;

}; // class InflowPattern

} // namespace Antares::Component::Datagrid::Renderer

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_WATERVALUES_H__
