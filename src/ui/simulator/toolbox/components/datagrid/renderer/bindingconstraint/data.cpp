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
#include "data.h"

#include "data.h"
#include <antares/study.h>
#include <antares/wx-wrapper.h>
#include <yuni/core/math.h>
#include <wx/window.h>

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
namespace BindingConstraint
{
Data::Data(wxWindow* control, const Antares::Data::BindingConstraint::Operator op) :
 pOperator(op),
 pControl(control),
 pZero(wxT("0.")),
 pType(Antares::Data::BindingConstraint::typeHourly)
{
    switch (pOperator)
    {
    case Antares::Data::BindingConstraint::opLess:
        pColumn = Antares::Data::BindingConstraint::columnInferior;
        break;
    case Antares::Data::BindingConstraint::opGreater:
        pColumn = Antares::Data::BindingConstraint::columnSuperior;
        break;
    case Antares::Data::BindingConstraint::opEquality:
        pColumn = Antares::Data::BindingConstraint::columnEquality;
        break;
    default:
        pColumn = Antares::Data::BindingConstraint::columnInferior;
        break;
    }

    OnStudyChanged.connect(this, &Data::onStudyChanged);
}

Data::~Data()
{
    destroyBoundEvents();
}

int Data::width() const
{
    return !study ? 0 : (uint)study->uiinfo->byOperator[pOperator][pType].size();
}

int Data::height() const
{
    return (pType == Antares::Data::BindingConstraint::typeHourly) ? 8760 : 365;
}

wxString Data::rowCaption(int row) const
{
    if (!study)
        return wxEmptyString;
    switch (pType)
    {
    case Antares::Data::BindingConstraint::typeHourly:
        if (row < study->calendar.maxHoursInYear)
            return wxStringFromUTF8(study->calendar.text.hours[row]);
        break;
    case Antares::Data::BindingConstraint::typeDaily:
    case Antares::Data::BindingConstraint::typeWeekly:
        if (row < study->calendar.maxDaysInYear)
            return wxStringFromUTF8(study->calendar.text.daysYear[row]);
        break;
    case Antares::Data::BindingConstraint::typeUnknown:
    case Antares::Data::BindingConstraint::typeMax:
        break;
    }
    return wxEmptyString;
}

Date::Precision Data::precision()
{
    switch (pType)
    {
    case Antares::Data::BindingConstraint::typeHourly:
        return Date::hourly;
    default:
        return Date::daily;
    }
}

double Data::cellNumericValue(int x, int y) const
{
    if (!(!study))
    {
        assert(study->uiinfo);
        auto* bindingconstraint = (study->uiinfo->byOperator[pOperator][pType][x]);
        assert(bindingconstraint);
        if (bindingconstraint)
        {
            auto& matrix = bindingconstraint->matrix();
            matrix.forceReload(true);
            assert((uint)pColumn < matrix.width);
            assert((uint)y < matrix.height);
            return matrix.entry[pColumn][y];
        }
    }
    return 0.;
}

wxString Data::cellValue(int x, int y) const
{
    return DoubleToWxString(cellNumericValue(x, y));
}

wxString Data::columnCaption(int x) const
{
    if (!study)
        return wxEmptyString;
    auto* constraint = (study->uiinfo->byOperator[pOperator][pType][x]);
    return constraint ? wxStringFromUTF8(constraint->name()) : wxString();
}

void Data::applyLayerFiltering(size_t layerID, VGridHelper* gridHelper)
{
    int last = 0; // The count of valid cols
    for (int x = 0; x < gridHelper->virtualSize.x; ++x)
    {
        // The current constraint
        Antares::Data::BindingConstraint* constraint
          = study->uiinfo->byOperator[pOperator][pType][x];

        if (constraint->hasAllWeightedLinksOnLayer(layerID))
        {
            gridHelper->indicesCols[last] = gridHelper->indicesCols[x];
            ++last;
        }
    }
    // gridHelper->indicesCols.resize(last);
    gridHelper->virtualSize.x = last;

    /*last = 5;// The count of valid rows
    for (int r = 5; r != gridHelper->virtualSize.y; ++r)
    {
    auto* lnk = study->uiinfo->link((uint)(r - 5));
    assert(lnk);
    assert(lnk->from);
    assert(lnk->with);
    if (lnk->isVisibleOnLayer(layerIt->first))
    {
    gridHelper->indicesRows[last] = r;
    ++last;
    }
    }
    gridHelper->virtualSize.y = last;*/
}

IRenderer::CellStyle Data::cellStyle(int, int y) const
{
    return ((y % 2) ? IRenderer::cellStyleDefaultAlternate : IRenderer::cellStyleDefault);
}

bool Data::cellValue(int x, int y, const String& value)
{
    if (!study)
        return false;

    auto* constraint = (study->uiinfo->byOperator[pOperator][pType][x]);
    if (constraint)
    {
        double v;
        if (value.to(v))
        {
            constraint->matrix().entry[pColumn][y] = v;
            constraint->matrix().markAsModified();
            return true;
        }
    }
    return false;
}

void Data::bindingConstraintTypeChanged(Antares::Data::BindingConstraint::Type type)
{
    pType = type;
}

void Data::onStudyChanged(Antares::Data::Study&)
{
    invalidate = true;
}

wxColour Data::horizontalBorderColor(int x, int y) const
{
    // Getting informations about the next hour
    // (because the returned color is about the bottom border of the cell,
    // so the next hour for the user)
    if (!(!study) and (y + 1 < Date::Calendar::maxHoursInYear))
    {
        auto& hourinfo = study->calendar.hours[y + 1];

        if (hourinfo.firstHourInMonth)
            return Default::BorderMonthSeparator();
        if (hourinfo.firstHourInDay)
            return Default::BorderDaySeparator();
    }
    return IRenderer::verticalBorderColor(x, y);
}

} // namespace BindingConstraint
} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
