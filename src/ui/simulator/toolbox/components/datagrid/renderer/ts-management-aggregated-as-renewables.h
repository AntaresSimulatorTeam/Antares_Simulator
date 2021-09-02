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

#pragma once

#include "ts-management.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
class TSmanagementRenewableAggregated final : public TSmanagement
{
public:
    TSmanagementRenewableAggregated();
    virtual ~TSmanagementRenewableAggregated();

    virtual int width() const override
    {
        return 5;
    }

    virtual wxString columnCaption(int colIndx) const override;

    // virtual wxString rowCaption(int rowIndx) const;

    Antares::Data::TimeSeries getTSfromColumn(int col) const override;

    virtual wxString cellValue(int x, int y) const;

    virtual double cellNumericValue(int x, int y) const;

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

    // void control(wxWindow* control)
    // {
    //     pControl = control;
    // }

// protected:
//     wxWindow* pControl;

private:
    bool cellValueForRenewables(int x, int y, const double v) override;
    double TSmanagement::cellNumericValueForRenewables(int x, int y) const override;
    wxString cellValueForRenewables(int x, int y) const override;
    IRenderer::CellStyle cellStyleForRenewables(int x, int y) const;

    void onSimulationTSManagementChanged();

}; // class TSmanagementRenewableAggregated

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
