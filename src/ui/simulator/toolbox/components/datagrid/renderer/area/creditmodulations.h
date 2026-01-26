// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_CREDITMODULATIONS_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_CREDITMODULATIONS_H__

#include "../area.h"
#include "../matrix.h"
#include <antares/date/date.h>
#include <antares/study/parts/wind/prepro.h>

namespace Antares::Component::Datagrid::Renderer
{
class CreditModulations final: public Renderer::Matrix<double, double, 2>,
                               public Renderer::ARendererArea
{
public:
    using MatrixAncestorType = Renderer::Matrix<double, double, 2>;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    CreditModulations(wxWindow* control, Toolbox::InputSelector::Area* notifier);
    //! Destructor
    virtual ~CreditModulations();

    //@}

    virtual int width() const
    {
        return 101;
    }

    virtual int height() const
    {
        return 2;
    }

    virtual wxString columnCaption(int colIndx) const;

    virtual wxString rowCaption(int rowIndx) const;

    virtual wxString cellValue(int x, int y) const;

    virtual double cellNumericValue(int x, int y) const;

    virtual bool cellValue(int, int, const Yuni::String&);

    virtual void resetColors(int, int, wxColour&, wxColour&) const
    { /*Do nothing*/
    }

    virtual IRenderer::CellStyle cellStyle(int col, int row) const;

    virtual Date::Precision precision()
    {
        return Date::daily;
    }

    virtual bool valid() const
    {
        return MatrixAncestorType::valid();
    }

    virtual uint maxWidthResize() const
    {
        return 0;
    }

    virtual uint maxHeightResize() const
    {
        return 0;
    }

protected:
    virtual void internalAreaChanged(Antares::Data::Area* area);
    //! Event: the study has been closed
    virtual void onStudyClosed() override;
    //! Event: the study has been loaded
    virtual void onStudyLoaded() override;

}; // class CreditModulations

} // namespace Antares::Component::Datagrid::Renderer

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_CREDITMODULATIONS_H__
