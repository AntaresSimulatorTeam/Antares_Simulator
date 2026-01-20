// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_CONVERTION_HXX__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_CONVERTION_HXX__

#include <antares/study/xcast/xcast.h>

namespace Antares::Component::Datagrid::Renderer
{
template<enum Data::TimeSeriesType T>
XCastConversion<T>::XCastConversion(wxWindow* control, Toolbox::InputSelector::Area* notifier):
    MatrixAncestorType(control),
    Renderer::ARendererArea(control, notifier)
{
}

template<enum Data::TimeSeriesType T>
XCastConversion<T>::~XCastConversion()
{
    destroyBoundEvents();
}

template<enum Data::TimeSeriesType T>
inline wxString XCastConversion<T>::columnCaption(int colIndx) const
{
    return wxString() << wxT(" point ") << (colIndx + 1) << wxT(' ');
}

template<enum Data::TimeSeriesType T>
inline wxString XCastConversion<T>::cellValue(int x, int y) const
{
    return MatrixAncestorType::cellValue(x + 1, y);
}

template<enum Data::TimeSeriesType T>
double XCastConversion<T>::cellNumericValue(int x, int y) const
{
    return MatrixAncestorType::cellNumericValue(x + 1, y);
}

template<enum Data::TimeSeriesType T>
inline bool XCastConversion<T>::cellValue(int x, int y, const Yuni::String& value)
{
    return MatrixAncestorType::cellValue(x + 1, y, value);
}

template<enum Data::TimeSeriesType T>
void XCastConversion<T>::internalAreaChanged(Data::Area* area)
{
    if (area)
    {
        auto* xcastData = area->xcastData<T>();
        this->matrix(&(xcastData->conversion));
    }
    else
    {
        this->matrix(NULL);
    }
    Renderer::ARendererArea::internalAreaChanged(area);
}

template<enum Data::TimeSeriesType T>
IRenderer::CellStyle XCastConversion<T>::cellStyle(int col, int row) const
{
    double b = MatrixAncestorType::cellNumericValue(col + 1, 0);
    return (Yuni::Math::Abs(b) >= 1.0e+19)
               || (col > 0 && b <= MatrixAncestorType::cellNumericValue(col, 0))
             ? cellStyleError
             : MatrixAncestorType::cellStyle(col, row);
}

template<enum Data::TimeSeriesType T>
wxString XCastConversion<T>::rowCaption(int rowIndx) const
{
    return (rowIndx == 0) ? wxT("IN") : wxT("OUT");
}

template<enum Data::TimeSeriesType T>
bool XCastConversion<T>::onMatrixResize(uint, uint, uint& newX, uint&)
{
    if (newX > Data::XCast::conversionMaxPoints)
    {
        return false;
    }
    auto& matrix = (*pMatrix);
    matrix[matrix.width - 1][0] = 0;
    matrix[matrix.width - 1][1] = 0;
    matrix.resizeWithoutDataLost(newX + 2, 2);
    matrix[matrix.width - 1][0] = +std::numeric_limits<float>::max();
    matrix.markAsModified();
    MarkTheStudyAsModified();
    return false;
}

template<enum Data::TimeSeriesType T>
void XCastConversion<T>::onMatrixLoad()
{
    auto& matrix = *pMatrix;
    if (matrix.height != 2)
    {
        // It may happen when the file is not loaded
        matrix.reset(3, 2);
        matrix.markAsModified();
        MarkTheStudyAsModified();
    }
    if (matrix.width >= 3 && matrix.width <= Data::XCast::conversionMaxPoints)
    {
        matrix[0][0] = -std::numeric_limits<float>::max();
        matrix[0][1] = matrix[1][1];
        matrix[matrix.width - 1][0] = +std::numeric_limits<float>::max();
        matrix[matrix.width - 1][1] = matrix[matrix.width - 2][1];
    }
    else
    {
        matrix.reset(3, 2);
        matrix[0][0] = -std::numeric_limits<float>::max();
        matrix[0][1] = 0.f;
        matrix[1][0] = 0.f;
        matrix[1][1] = 0.f;
        matrix[2][0] = +std::numeric_limits<float>::max();
        matrix[2][1] = 0.f;
    }
}

template<enum Data::TimeSeriesType T>
void XCastConversion<T>::onStudyClosed()
{
    Renderer::Matrix<float>::onStudyClosed();
    Renderer::ARendererArea::onStudyClosed();
}

template<enum Data::TimeSeriesType T>
void XCastConversion<T>::onStudyLoaded()
{
    Renderer::Matrix<float>::onStudyLoaded();
    Renderer::ARendererArea::onStudyLoaded();
}

} // namespace Antares::Component::Datagrid::Renderer

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_CONVERTION_HXX__
