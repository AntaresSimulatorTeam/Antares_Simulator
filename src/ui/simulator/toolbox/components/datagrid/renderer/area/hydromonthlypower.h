/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_HYDRO_MONTHLY_POWER_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_HYDRO_MONTHLY_POWER_H__

#include "../area.h"
#include "../matrix.h"
#include <antares/date/date.h>
#include <antares/study/parts/wind/prepro.h>

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{

class HydroMonthlyHours final : public Renderer::Matrix<double, double, 2>,
                                public Renderer::ARendererArea
{
public:
    using MatrixAncestorType = Renderer::Matrix<double, double, 2>;
    enum class HoursType
    {
        Generation = 0,
        Pumping
    };

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    HydroMonthlyHours(wxWindow* control, Toolbox::InputSelector::Area* notifier, HoursType type);
    //! Destructor
    virtual ~HydroMonthlyHours();
    //@}

    virtual int width() const
    {
        return 1;
    }
    virtual int height() const
    {
        return DAYS_PER_YEAR;
    }

    HoursType hoursType;

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

}; // class HydroMonthlyHoursGen

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_HYDRO_MONTHLY_POWER_H__
