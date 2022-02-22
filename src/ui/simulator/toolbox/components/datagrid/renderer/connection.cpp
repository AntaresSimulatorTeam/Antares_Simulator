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

#include "connection.h"
#include "../../../libs/antares/constants.h"
#include <antares/study/area/constants.h>
#include <yuni/core/math.h>

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
Connection::Connection(wxWindow* control, Toolbox::InputSelector::Connections* notifier) :
 Renderer::Matrix<>(control), pControl(control)
{
    if (notifier)
        notifier->onConnectionChanged.connect(this, &Connection::onConnectionChanged);
}

Connection::~Connection()
{
    destroyBoundEvents();
}

wxString Connection::rowCaption(int row) const
{
    if (!study || row >= study->calendar.maxHoursInYear)
        return wxString() << (row + 1);
    return wxStringFromUTF8(study->calendar.text.hours[row]);
}

void Connection::onConnectionChanged(Data::AreaLink* link)
{
    setMatrix(link);
    if (link)
        useLoopFlow = link->useLoopFlow;
    if (pControl)
    {
        pControl->InvalidateBestSize();
        pControl->Refresh();
    }
}

wxColour Connection::horizontalBorderColor(int x, int y) const
{
    // Getting informations about the next hour
    // (because the returned color is about the bottom border of the cell,
    // so the next hour for the user)
    if (!(!study) && y + 1 < Date::Calendar::maxHoursInYear)
    {
        auto& hourinfo = study->calendar.hours[y + 1];

        if (hourinfo.firstHourInMonth)
            return Default::BorderMonthSeparator();
        if (hourinfo.firstHourInDay)
            return Default::BorderDaySeparator();
    }
    return IRenderer::verticalBorderColor(x, y);
}

// ===========================
// Parameters grid renderer
// ===========================
connectionParameters::connectionParameters(wxWindow* parent,
                                           Toolbox::InputSelector::Connections* notifier) :
 Connection(parent, notifier)
{
}

wxString connectionParameters::columnCaption(int colIndx) const
{
    switch (colIndx)
    {
    case Data::fhlHurdlesCostDirect:
        return wxT(" HURDLES COST \nDirect");
    case Data::fhlHurdlesCostIndirect:
        return wxT(" HURDLES COST \nIndirect");
    case Data::fhlImpedances:
        return wxT(" IMPEDANCES ");
    case Data::fhlLoopFlow:
        return wxT(" LOOP FLOW ");
    case Data::fhlPShiftMinus:
        return wxT(" P.SHIFT MIN ");
    case Data::fhlPShiftPlus:
        return wxT(" P.SHIFT MAX ");
    }
    return wxString();
}

bool connectionParameters::cellValue(int x, int y, const Yuni::String& value)
{
    switch (x)
    {
    case Data::fhlHurdlesCostDirect:
    case Data::fhlHurdlesCostIndirect:
    default:
    {
        double v;
        if (!value.to(v))
            return false;
        if (Math::Abs(v) < LINK_MINIMAL_HURDLE_COSTS_NOT_NULL)
            return Renderer::Matrix<>::cellValue(x, y, "0");
        break;
    }
    }

    return Renderer::Matrix<>::cellValue(x, y, value);
}

static bool checkLoopFlow(const Antares::Matrix<>* direct_ntc,
                          const Antares::Matrix<>* indirect_ntc,
                          double loopflow,
                          int row,
                          bool useLoopFlow)
{
    if (!useLoopFlow)
        return true;

    for (int ts = 0; ts < direct_ntc->width; ts++)
    {
        const double ntcDirect = direct_ntc->entry[ts][row];
        const double ntcIndirect = indirect_ntc->entry[ts][row];

        if (ntcDirect < loopflow)
        {
            return false;
        }
        if (loopflow < 0. && std::abs(loopflow) > ntcIndirect)
        {
            return false;
        }
    }
    return true;
}

static bool checkLoopFlowNTCDirect(double ntcDirect, double loopFlow, bool useLoopFlow)
{
    if (!useLoopFlow)
        return true;
    return ntcDirect >= loopFlow;
}

static bool checkLoopFlowNTCIndirect(double ntcIndirect, double loopFlow, bool useLoopFlow)
{
    if (!useLoopFlow)
        return true;
    return (loopFlow >= 0) || (std::abs(loopFlow) <= ntcIndirect);
}

IRenderer::CellStyle connectionParameters::cellStyle(int col, int row) const
{
    double cellvalue = cellNumericValue(col, row);
    switch (col)
    {
    case Data::fhlHurdlesCostDirect:
    {
        double cellvalueHrdlCostIndirect = cellNumericValue(col + 1, row);
        double sumCellValues = cellvalue + cellvalueHrdlCostIndirect;
        if (sumCellValues < 0.)
            return IRenderer::cellStyleError;
        if (sumCellValues >= 0. && cellvalue < 0.)
            return IRenderer::cellStyleWarning;
        break;
    }
    case Data::fhlHurdlesCostIndirect:
    {
        double cellvalueHrdlCostDirect = cellNumericValue(col - 1, row);
        double sumCellValues = cellvalueHrdlCostDirect + cellvalue;
        if (sumCellValues < 0.)
            return IRenderer::cellStyleError;
        if (sumCellValues >= 0. && cellvalue < 0.)
            return IRenderer::cellStyleWarning;
        break;
    }
    case Data::fhlImpedances:
        return (cellvalue < 0.) ? IRenderer::cellStyleWarning
                                : Renderer::Matrix<>::cellStyle(col, row);
    case Data::fhlLoopFlow:
    {
        if (!checkLoopFlow(direct_ntc_, indirect_ntc_, cellvalue, row, useLoopFlow))
            return IRenderer::cellStyleWarning;
        break;
    }
    case Data::fhlPShiftMinus:
    {
        double pShiftMax = cellNumericValue(Data::fhlPShiftPlus, row);
        if (cellvalue > pShiftMax)
        {
            return IRenderer::cellStyleError;
        }

        break;
    }
    case Data::fhlPShiftPlus:
    {
        double pShiftMin = cellNumericValue(Data::fhlPShiftMinus, row);
        if (cellvalue < pShiftMin)
        {
            return IRenderer::cellStyleError;
        }
        break;
    }
    }
    return Renderer::Matrix<>::cellStyle(col, row);
}

void connectionParameters::setMatrix(Data::AreaLink* link)
{
    matrix(link ? &(link->parameters) : nullptr);

    direct_ntc_ = link ? &(link->directCapacities) : nullptr;
    indirect_ntc_ = link ? &(link->indirectCapacities) : nullptr;
}

// ===========================
// NTC grid renderer
// ===========================
connectionNTC::connectionNTC(wxWindow* parent, Toolbox::InputSelector::Connections* notifier) :
 Connection(parent, notifier)
{
}

bool connectionNTC::cellValue(int x, int y, const Yuni::String& value)
{
    double v;
    if (!value.to(v))
        return false;
    if (v < 0.)
        return Renderer::Matrix<>::cellValue(x, y, "0");

    return Renderer::Matrix<>::cellValue(x, y, value);
}

// ----------------
// Direct
// ----------------
connectionNTCdirect::connectionNTCdirect(wxWindow* parent,
                                         Toolbox::InputSelector::Connections* notifier) :
 connectionNTC(parent, notifier)
{
}
void connectionNTCdirect::setMatrix(Data::AreaLink* link)
{
    matrix(link ? &(link->directCapacities) : nullptr);
    parameters = link ? &(link->parameters) : nullptr;
}

IRenderer::CellStyle connectionNTCdirect::cellStyle(int col, int row) const
{
    if (!parameters)
        return Renderer::Matrix<>::cellStyle(col, row);

    const double loopFlow = (*parameters)[Data::fhlLoopFlow][row];
    const double ntcDirect = Renderer::Matrix<>::cellNumericValue(col, row);
    if (!checkLoopFlowNTCDirect(ntcDirect, loopFlow, useLoopFlow))
        return IRenderer::cellStyleWarning;

    return Renderer::Matrix<>::cellStyle(col, row);
}
// ----------------
// Indirect
// ----------------
connectionNTCindirect::connectionNTCindirect(wxWindow* parent,
                                             Toolbox::InputSelector::Connections* notifier) :
 connectionNTC(parent, notifier)
{
}
void connectionNTCindirect::setMatrix(Data::AreaLink* link)
{
    matrix(link ? &(link->indirectCapacities) : nullptr);
    parameters = link ? &(link->parameters) : nullptr;
}

IRenderer::CellStyle connectionNTCindirect::cellStyle(int col, int row) const
{
    if (!parameters)
        return Renderer::Matrix<>::cellStyle(col, row);

    const double loopFlow = (*parameters)[Data::fhlLoopFlow][row];
    const double ntcInirect = Renderer::Matrix<>::cellNumericValue(col, row);
    if (!checkLoopFlowNTCIndirect(ntcInirect, loopFlow, useLoopFlow))
        return IRenderer::cellStyleWarning;

    return Renderer::Matrix<>::cellStyle(col, row);
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
