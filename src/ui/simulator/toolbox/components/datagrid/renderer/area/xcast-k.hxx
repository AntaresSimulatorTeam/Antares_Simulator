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
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <antares/study/xcast/xcast.h>

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
template<enum Data::TimeSeriesType T>
XCastK<T>::XCastK(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
 MatrixAncestorType(control), Renderer::ARendererArea(control, notifier)
{
}

template<enum Data::TimeSeriesType T>
XCastK<T>::~XCastK()
{
    destroyBoundEvents();
}

template<enum Data::TimeSeriesType T>
wxString XCastK<T>::columnCaption(int colIndx) const
{
    return (colIndx < 12) ? wxStringFromUTF8(Date::MonthToString(colIndx)) << wxT("  ")
                          : wxString();
}

template<enum Data::TimeSeriesType T>
wxString XCastK<T>::cellValue(int x, int y) const
{
    return MatrixAncestorType::cellValue(x, y);
}

template<enum Data::TimeSeriesType T>
double XCastK<T>::cellNumericValue(int x, int y) const
{
    return MatrixAncestorType::cellNumericValue(x, y);
}

template<enum Data::TimeSeriesType T>
bool XCastK<T>::cellValue(int x, int y, const Yuni::String& value)
{
    return MatrixAncestorType::cellValue(x, y, value);
}

template<enum Data::TimeSeriesType T>
void XCastK<T>::internalAreaChanged(Data::Area* area)
{
    if (area)
    {
        auto* xcastData = area->xcastData<T>();
        this->matrix(&(xcastData->K));
    }
    else
        this->matrix(nullptr);
    // parent
    Renderer::ARendererArea::internalAreaChanged(area);
}

template<enum Data::TimeSeriesType T>
inline IRenderer::CellStyle XCastK<T>::cellStyle(int col, int row) const
{
    return MatrixAncestorType::cellStyle(col, row);
}

template<enum Data::TimeSeriesType T>
inline wxString XCastK<T>::rowCaption(int rowIndx) const
{
    return wxString() << rowIndx;
}

template<enum Data::TimeSeriesType T>
inline int XCastK<T>::width() const
{
    return MatrixAncestorType::width();
}

template<enum Data::TimeSeriesType T>
inline int XCastK<T>::height() const
{
    return MatrixAncestorType::height();
}

template<enum Data::TimeSeriesType T>
inline uint XCastK<T>::maxWidthResize() const
{
    return 0;
}

template<enum Data::TimeSeriesType T>
inline bool XCastK<T>::valid() const
{
    return MatrixAncestorType::valid();
}

template<enum Data::TimeSeriesType T>
void XCastK<T>::onStudyClosed()
{
    Renderer::Matrix<float>::onStudyClosed();
    Renderer::ARendererArea::onStudyClosed();
}

template<enum Data::TimeSeriesType T>
void XCastK<T>::onStudyLoaded()
{
    Renderer::Matrix<float>::onStudyLoaded();
    Renderer::ARendererArea::onStudyLoaded();
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
