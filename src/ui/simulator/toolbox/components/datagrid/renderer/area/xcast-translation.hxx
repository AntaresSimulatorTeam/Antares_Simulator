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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_TRANSLATION_HXX__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_TRANSLATION_HXX__

#include <antares/study/xcast/xcast.h>
#include <antares/date/date.h>

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
template<enum Data::TimeSeriesType T>
XCastTranslation<T>::XCastTranslation(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
 MatrixAncestorType(control), Renderer::ARendererArea(control, notifier)
{
}

template<enum Data::TimeSeriesType T>
XCastTranslation<T>::~XCastTranslation()
{
    destroyBoundEvents();
}

template<enum Data::TimeSeriesType T>
inline wxString XCastTranslation<T>::columnCaption(int colIndx) const
{
    return MatrixAncestorType::columnCaption(colIndx);
}

template<enum Data::TimeSeriesType T>
wxString XCastTranslation<T>::cellValue(int x, int y) const
{
    return MatrixAncestorType::cellValue(x, y);
}

template<enum Data::TimeSeriesType T>
inline double XCastTranslation<T>::cellNumericValue(int x, int y) const
{
    return MatrixAncestorType::cellNumericValue(x, y);
}

template<enum Data::TimeSeriesType T>
inline bool XCastTranslation<T>::cellValue(int x, int y, const Yuni::String& value)
{
    return MatrixAncestorType::cellValue(x, y, value);
}

template<enum Data::TimeSeriesType T>
void XCastTranslation<T>::internalAreaChanged(Data::Area* area)
{
    if (area)
    {
        auto* xcastData = area->xcastData<T>();
        this->matrix(&(xcastData->translation));
    }
    else
        this->matrix(nullptr);
    // parent
    Renderer::ARendererArea::internalAreaChanged(area);
}

template<enum Data::TimeSeriesType T>
inline IRenderer::CellStyle XCastTranslation<T>::cellStyle(int col, int row) const
{
    return MatrixAncestorType::cellStyle(col, row);
}

template<enum Data::TimeSeriesType T>
inline wxString XCastTranslation<T>::rowCaption(int rowIndx) const
{
    return MatrixAncestorType::rowCaption(rowIndx);
}

template<enum Data::TimeSeriesType T>
wxColour XCastTranslation<T>::horizontalBorderColor(int x, int y) const
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

template<enum Data::TimeSeriesType T>
void XCastTranslation<T>::onStudyClosed()
{
    Renderer::Matrix<int32_t>::onStudyClosed();
    Renderer::ARendererArea::onStudyClosed();
}

template<enum Data::TimeSeriesType T>
void XCastTranslation<T>::onStudyLoaded()
{
    Renderer::Matrix<int32_t>::onStudyLoaded();
    Renderer::ARendererArea::onStudyLoaded();
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_TRANSLATION_HXX__
