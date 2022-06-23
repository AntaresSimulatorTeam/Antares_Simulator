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
inline XCastCoefficients<T>::XCastCoefficients(wxWindow* control,
                                               Toolbox::InputSelector::Area* notifier) :
 MatrixAncestorType(control), Renderer::ARendererArea(control, notifier)
{
}

template<enum Data::TimeSeries T>
inline XCastCoefficients<T>::~XCastCoefficients()
{
    destroyBoundEvents();
}

template<enum Data::TimeSeries T>
wxString XCastCoefficients<T>::columnCaption(int colIndx) const
{
    static const wxString captions[Data::XCast::dataMax] = {wxT("        alpha        "),
                                                            wxT("        beta         "),
                                                            wxT("       gamma       "),
                                                            wxT("        delta        "),
                                                            wxT("        theta        "),
                                                            wxT("    mu    ")};
    return colIndx < Data::XCast::dataMax ? captions[colIndx] : wxString();
}

template<enum Data::TimeSeries T>
wxString XCastCoefficients<T>::cellValue(int x, int y) const
{
    return MatrixAncestorType::cellValue(x, y);
}

template<enum Data::TimeSeries T>
double XCastCoefficients<T>::cellNumericValue(int x, int y) const
{
    return MatrixAncestorType::cellNumericValue(x, y);
}

template<enum Data::TimeSeries T>
inline bool XCastCoefficients<T>::cellValue(int x, int y, const Yuni::String& value)
{
    using namespace Antares::Data;

    if (!pArea)
        return false;
    auto& xcast = *(pArea->xcastData<T>());
    float v;
    if (!value.to(v))
        return false;
    if ((uint)x >= xcast.data.width || (uint)y >= xcast.data.height)
        return false;

    switch (x)
    {
    case XCast::dataCoeffAlpha:
    {
        switch (xcast.distribution)
        {
        case XCast::dtUniform:
            break;
        case XCast::dtBeta:
        {
            if (v <= 0.f)
                return MatrixAncestorType::cellValue(x, y, "0");
            break;
        }
        case XCast::dtNormal:
            break;
        default:
        {
            if (v < 1.f || v > 50.f)
                return MatrixAncestorType::cellValue(x, y, "1");
        }
        }
        break;
    }
    case XCast::dataCoeffBeta:
    {
        switch (xcast.distribution)
        {
        case XCast::dtUniform:
            break;
        default:
        {
            if (v <= 0.f)
                return MatrixAncestorType::cellValue(x, y, "0");
            break;
        }
        }
        break;
    }
    case XCast::dataCoeffGamma:
    {
        switch (xcast.distribution)
        {
        case XCast::dtUniform:
        {
            // if (v > xcast.data.entry[XCast::dataCoeffDelta][y])
            //	return MatrixAncestorType::cellValue(x, y,
            // String(xcast.data.entry[XCast::dataCoeffDelta][y]));
            break;
        }
        case XCast::dtBeta:
        {
            // if (v >= xcast.data.entry[XCast::dataCoeffDelta][y])
            //	return MatrixAncestorType::cellValue(x, y, "0");
            break;
        }
        case XCast::dtNormal:
            break;
        default:
            break;
        }
        break;
    }
    case XCast::dataCoeffDelta:
    {
        switch (xcast.distribution)
        {
        case XCast::dtUniform:
        {
            // if (v < xcast.data.entry[XCast::dataCoeffGamma][y])
            //	return MatrixAncestorType::cellValue(x, y,
            // xcast.data.entry[XCast::dataCoeffGamma][y]);
            break;
        }
        case XCast::dtBeta:
        {
            // if (v <= xcast.data.entry[XCast::dataCoeffGamma][y])
            //	return MatrixAncestorType::cellValue(x, y, "1");
            break;
        }
        default:
            break;
        }
        break;
    }

    case XCast::dataCoeffTheta:
    {
        if (v < 0.f)
            return MatrixAncestorType::cellValue(x, y, "0");
        break;
    }
    case XCast::dataCoeffMu:
    {
        if (v < 1.f)
            return MatrixAncestorType::cellValue(x, y, "1");
        if (v > 23.f)
            return MatrixAncestorType::cellValue(x, y, "23");
        break;
    }
    }
    return MatrixAncestorType::cellValue(x, y, value);
}

template<enum Data::TimeSeries T>
void XCastCoefficients<T>::internalAreaChanged(Data::Area* area)
{
    if (area)
    {
        Data::XCast* xcastData = area->xcastData<T>();
        this->matrix(&(xcastData->data));
    }
    else
        this->matrix(NULL);
    Renderer::ARendererArea::internalAreaChanged(area);
}

template<enum Data::TimeSeries T>
IRenderer::CellStyle XCastCoefficients<T>::cellStyle(int col, int row) const
{
    //   law \  coeff       alpha         beta     gamma    delta
    //
    //     uniform            ?           ?         R        R
    //     beta               >0          >0        R        R
    //     normal             R           >0        R        R
    //     weibull       1 <= x <= 50     >0        R        ?
    //     gamma         1 <= x <= 50     >0        R        ?
    //
    //  R : -inf < x < +inf
    //  ? : whatever (disabled)
    //
    //  mu : 1 <= mu <= 23
    //  if uniform || beta  then  gamma < delta
    //
    using namespace Antares::Data;
    if (!pArea)
        return IRenderer::cellStyleDefault;
    XCast& xcast = *(pArea->xcastData<T>());
    if ((uint)col >= xcast.data.width || (uint)row >= xcast.data.height)
        return IRenderer::cellStyleDefault;

    float d = xcast.data[col][row];
    switch (col)
    {
    case XCast::dataCoeffAlpha:
    {
        switch (xcast.distribution)
        {
        case XCast::dtUniform:
            return IRenderer::cellStyleDefaultDisabled;
        case XCast::dtBeta:
        {
            if (d <= 0.f)
                return IRenderer::cellStyleError;
            break;
        }
        case XCast::dtNormal:
            break;
        default:
        {
            if (d < 1.f || d > 50.f)
                return IRenderer::cellStyleError;
        }
        }
        break;
    }
    case XCast::dataCoeffBeta:
    {
        switch (xcast.distribution)
        {
        case XCast::dtUniform:
            return IRenderer::cellStyleDefaultDisabled;
        case XCast::dtNormal:
        {
            if (d < 0.)
                return IRenderer::cellStyleError;
            break;
        }
        default:
        {
            if (d <= 0.)
                return IRenderer::cellStyleError;
            break;
        }
        }
        break;
    }
    case XCast::dataCoeffGamma:
    {
        switch (xcast.distribution)
        {
        case XCast::dtUniform:
        {
            if (d > xcast.data.entry[XCast::dataCoeffDelta][row])
                return IRenderer::cellStyleError;
            break;
        }
        case XCast::dtBeta:
        {
            if (d >= xcast.data.entry[XCast::dataCoeffDelta][row])
                return IRenderer::cellStyleError;
            break;
        }
        case XCast::dtNormal:
            return IRenderer::cellStyleDefaultDisabled;
        default:
            break;
        }
        break;
    }
    case XCast::dataCoeffDelta:
    {
        switch (xcast.distribution)
        {
        case XCast::dtUniform:
        {
            if (d < xcast.data.entry[XCast::dataCoeffGamma][row])
                return IRenderer::cellStyleError;
            break;
        }
        case XCast::dtBeta:
        {
            if (d <= xcast.data.entry[XCast::dataCoeffGamma][row])
                return IRenderer::cellStyleError;
            break;
        }
        default:
            return IRenderer::cellStyleDefaultDisabled;
        }
        break;
    }
    case XCast::dataCoeffTheta:
    {
        if (d < 0.f)
            return IRenderer::cellStyleError;
        break;
    }
    case XCast::dataCoeffMu:
    {
        if (d < 1.f || d > 23.f)
            return IRenderer::cellStyleError;
        break;
    }
    default:
        break;
    }
    return Yuni::Math::Zero(d) ? IRenderer::cellStyleDefaultDisabled : IRenderer::cellStyleDefault;
}

template<enum Data::TimeSeries T>
wxString XCastCoefficients<T>::rowCaption(int rowIndx) const
{
    return ((uint)rowIndx < 12) ? wxString() << (1 + rowIndx) << wxT(" - ")
                                             << wxStringFromUTF8(Date::MonthToString(rowIndx))
                                : wxString();
}

template<enum Data::TimeSeries T>
inline Date::Precision XCastCoefficients<T>::precision()
{
    return Date::monthly;
}

template<enum Data::TimeSeries T>
void XCastCoefficients<T>::onStudyClosed()
{
    Renderer::Matrix<float>::onStudyClosed();
    Renderer::ARendererArea::onStudyClosed();
}

template<enum Data::TimeSeries T>
void XCastCoefficients<T>::onStudyLoaded()
{
    Renderer::Matrix<float>::onStudyLoaded();
    Renderer::ARendererArea::onStudyLoaded();
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
