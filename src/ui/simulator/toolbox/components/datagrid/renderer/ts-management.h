/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_TS_MANAGEMENT_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_TS_MANAGEMENT_H__

#include <antares/wx-wrapper.h>
#include "../renderer.h"
#include <antares/date.h>
#include <yuni/core/event.h>
#include "../../../../application/study.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
class TSmanagement : public IRenderer
{
public:
    TSmanagement();
    virtual ~TSmanagement();

    virtual int width() const = 0;

    int height() const override
    {
        return 13;
    }

    virtual wxString columnCaption(int colIndx) const = 0;

    virtual wxString rowCaption(int rowIndx) const;

    virtual Antares::Data::TimeSeries getTSfromColumn(int col) const = 0; // gp : protected ?
    
    virtual wxString cellValue(int x, int y);

    virtual double cellNumericValue(int x, int y);

    virtual bool cellValue(int x, int y, const Yuni::String& value);

    virtual void resetColors(int, int, wxColour&, wxColour&) const
    {
        // Do nothing
    }

    virtual bool valid() const
    {
        return !(!study);
    }

    virtual uint maxWidthResize() const
    {
        return 0;
    }
    virtual IRenderer::CellStyle cellStyle(int col, int row) const;

    virtual wxColour horizontalBorderColor(int x, int y) const;

    void control(wxWindow* control)
    {
        pControl = control;
    }

protected:
    wxWindow* pControl;

private:
    virtual bool cellValueForRenewables(int x, int y, const double v) const = 0;
    virtual double cellNumericValueForRenewables(int x, int y) const = 0;
    virtual wxString cellValueForRenewables(int x, int y) const = 0;

    void onSimulationTSManagementChanged();

protected:
    bool pConversionToDoubleValid;
    uint pNumberTS;
    uint pRefreshSpan;
    double pValueToDouble;
    Antares::Data::Correlation::Mode pMode;


}; // class TSmanagement

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_TS_MANAGEMENT_H__
