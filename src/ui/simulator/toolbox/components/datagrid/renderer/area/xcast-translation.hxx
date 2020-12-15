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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_TRANSLATION_HXX__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_TRANSLATION_HXX__

#include <antares/study/xcast/xcast.h>
#include <antares/date.h>

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
template<enum Data::TimeSeries T>
XCastTranslation<T>::XCastTranslation(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
 MatrixAncestorType(control), Renderer::ARendererArea(control, notifier)
{
}

template<enum Data::TimeSeries T>
XCastTranslation<T>::~XCastTranslation()
{
    destroyBoundEvents();
}

template<enum Data::TimeSeries T>
inline wxString XCastTranslation<T>::columnCaption(int colIndx) const
{
    return MatrixAncestorType::columnCaption(colIndx);
}

template<enum Data::TimeSeries T>
wxString XCastTranslation<T>::cellValue(int x, int y) const
{
    return MatrixAncestorType::cellValue(x, y);
}

template<enum Data::TimeSeries T>
inline double XCastTranslation<T>::cellNumericValue(int x, int y) const
{
    return MatrixAncestorType::cellNumericValue(x, y);
}

template<enum Data::TimeSeries T>
inline bool XCastTranslation<T>::cellValue(int x, int y, const Yuni::String& value)
{
    return MatrixAncestorType::cellValue(x, y, value);
}

template<enum Data::TimeSeries T>
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

template<enum Data::TimeSeries T>
inline IRenderer::CellStyle XCastTranslation<T>::cellStyle(int col, int row) const
{
    return MatrixAncestorType::cellStyle(col, row);
}

template<enum Data::TimeSeries T>
inline wxString XCastTranslation<T>::rowCaption(int rowIndx) const
{
    return MatrixAncestorType::rowCaption(rowIndx);
}

template<enum Data::TimeSeries T>
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

template<enum Data::TimeSeries T>
void XCastTranslation<T>::onStudyClosed()
{
    Renderer::Matrix<yint32>::onStudyClosed();
    Renderer::ARendererArea::onStudyClosed();
}

template<enum Data::TimeSeries T>
void XCastTranslation<T>::onStudyLoaded()
{
    Renderer::Matrix<yint32>::onStudyLoaded();
    Renderer::ARendererArea::onStudyLoaded();
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_TRANSLATION_HXX__
