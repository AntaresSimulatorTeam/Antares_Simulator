/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_DSM_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_DSM_H__

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
class DSM final : public Renderer::Matrix<>, public Renderer::ARendererArea
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

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_DSM_H__
