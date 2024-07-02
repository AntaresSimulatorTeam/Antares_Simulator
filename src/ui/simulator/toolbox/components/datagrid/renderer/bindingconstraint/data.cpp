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
#include "data.h"

#include "data.h"
#include <antares/study/study.h>
#include <yuni/core/math.h>
#include <wx/window.h>
#include "antares/study/ui-runtimeinfos.h"

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

double Data::cellNumericValue(int x, [[maybe_unused]] int y) const
{
    if (!(!study))
    {
        assert(study->uiinfo);
        auto bindingconstraint = (study->uiinfo->byOperator[pOperator][pType][x]);
        assert(bindingconstraint);
        if (bindingconstraint)
        {
            //Deleted some code. UI is deprecated but not yet removed
            return 0;
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
    auto constraint = (study->uiinfo->byOperator[pOperator][pType][x]);
    return constraint ? wxStringFromUTF8(constraint->name()) : wxString();
}

void Data::applyLayerFiltering(size_t layerID, VGridHelper* gridHelper)
{
    int last = 0; // The count of valid cols
    for (int x = 0; x < gridHelper->virtualSize.x; ++x)
    {
        // The current constraint
        auto constraint
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

bool Data::cellValue(int x, [[maybe_unused]]int y, const String& value)
{
    if (!study)
        return false;

    auto constraint = (study->uiinfo->byOperator[pOperator][pType][x]);
    if (constraint)
    {
        double v;
        if (value.to(v))
        {
            //Deleted some code. UI is deprecated but not yet removed
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
