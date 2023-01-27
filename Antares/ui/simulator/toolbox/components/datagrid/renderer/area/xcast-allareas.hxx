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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_ALL_AREAS_HXX__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_ALL_AREAS_HXX__

#include "../../../../../application/study.h"
#include "../../component.h"
#include <antares/study/xcast/xcast.h>

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
template<enum Data::TimeSeries T>
inline XCastAllAreas<T>::XCastAllAreas(wxWindow* parent, Toolbox::InputSelector::Area* notifier) :
 pControl(parent), pNotifier(notifier)
{
    OnStudyAreaRename.connect(this, &XCastAllAreas<T>::onAreaRenamed);
}

template<enum Data::TimeSeries T>
inline XCastAllAreas<T>::~XCastAllAreas()
{
    destroyBoundEvents();
}

template<enum Data::TimeSeries T>
inline int XCastAllAreas<T>::width() const
{
    return 4;
}

template<enum Data::TimeSeries T>
inline int XCastAllAreas<T>::height() const
{
    auto study = Data::Study::Current::Get();
    return !study ? 0 : study->areas.size();
}

template<enum Data::TimeSeries T>
inline bool XCastAllAreas<T>::valid() const
{
    auto study = Data::Study::Current::Get();
    return !(!study) and study->areas.size() != 0;
}

template<enum Data::TimeSeries T>
wxString XCastAllAreas<T>::columnCaption(int colIndx) const
{
    switch (colIndx)
    {
    case 0:
        return wxT("      Capacity ");
    case 1:
        return wxT("Distribution");
    case 2:
        return wxT("Conversion");
    case 3:
        return wxT("              Translation");
    }
    return wxEmptyString;
}

template<enum Data::TimeSeries T>
wxString XCastAllAreas<T>::rowCaption(int rowIndx) const
{
    auto study = Data::Study::Current::Get();
    if (!study || (uint)rowIndx >= study->areas.size())
        return wxEmptyString;
    return wxStringFromUTF8(study->areas.byIndex[rowIndx]->name);
}

template<enum Data::TimeSeries T>
IRenderer::CellStyle XCastAllAreas<T>::cellStyle(int x, int y) const
{
    auto study = Data::Study::Current::Get();
    if (!study)
        return IRenderer::cellStyleDefaultCenterDisabled;

    auto& area = *(study->areas.byIndex[y]);
    auto& xcastData = *area.xcastData<T>();
    if (Yuni::Math::Zero(xcastData.capacity))
        return IRenderer::cellStyleDefaultCenterDisabled;
    switch (x)
    {
    case 0:
        return Yuni::Math::Zero(xcastData.capacity) ? IRenderer::cellStyleDefaultCenterDisabled
                                                    : IRenderer::cellStyleConstraintWeight;
    case 2:
        return xcastData.useConversion ? IRenderer::cellStyleDefaultCenter
                                       : IRenderer::cellStyleDefaultCenterDisabled;
    case 3:
    {
        switch (xcastData.useTranslation)
        {
        case Data::XCast::tsTranslationNone:
            return IRenderer::cellStyleDefaultCenterDisabled;
        default:
            return IRenderer::cellStyleDefaultCenter;
        }
        return IRenderer::cellStyleDefaultCenter;
    }
    }
    return IRenderer::cellStyleDefaultCenter;
}

template<enum Data::TimeSeries T>
wxColour XCastAllAreas<T>::cellBackgroundColor(int, int y) const
{
    auto study = Data::Study::Current::Get();
    if (!study)
        return wxColour(0, 0, 0);
    auto& area = *(study->areas.byIndex[y]);
    return wxColor(area.ui->color[0], area.ui->color[1], area.ui->color[2]);
}

template<enum Data::TimeSeries T>
wxString XCastAllAreas<T>::cellValue(int x, int y) const
{
    auto study = Data::Study::Current::Get();
    if (!study)
        return wxEmptyString;

    auto& area = *(study->areas.byIndex[y]);
    auto& xcastData = *area.xcastData<T>();
    switch (x)
    {
    case 0:
        return DoubleToWxString(xcastData.capacity);
    case 1:
        return wxStringFromUTF8(Data::XCast::DistributionToCString(xcastData.distribution));
    case 2:
        return xcastData.useConversion ? wxT("Yes") : wxT("No");
    case 3:
    {
        switch (xcastData.useTranslation)
        {
        case Data::XCast::tsTranslationBeforeConversion:
            return wxT("BEFORE scaling");
        case Data::XCast::tsTranslationAfterConversion:
            return wxT("AFTER scaling");
        default:
            return wxT("No");
        }
        return wxT("No");
    }
    }
    return wxEmptyString;
}

template<enum Data::TimeSeries T>
double XCastAllAreas<T>::cellNumericValue(int x, int y) const
{
    auto study = Data::Study::Current::Get();
    if (!study)
        return 0.;
    auto& area = *(study->areas.byIndex[y]);
    auto& xcastData = *area.xcastData<T>();
    switch (x)
    {
    case 0:
        return xcastData.capacity;
    case 1:
        return double(xcastData.distribution);
    case 2:
        return xcastData.useConversion ? 1. : 0.;
    case 3:
    {
        switch (xcastData.useTranslation)
        {
        case Data::XCast::tsTranslationBeforeConversion:
            return -1.;
        case Data::XCast::tsTranslationAfterConversion:
            return +1.;
        default:
            return 0.;
        }
        return 0.;
    }
    }
    return 0.;
}

template<enum Data::TimeSeries T>
bool XCastAllAreas<T>::cellValue(int x, int y, const Yuni::String& value)
{
    auto study = Data::Study::Current::Get();
    if (!study)
        return false;

    YString v = value;
    v.toLower();
    auto& area = *(study->areas.byIndex[y]);
    auto& xcastData = *area.xcastData<T>();
    switch (x)
    {
    case 0:
    {
        double d;
        if (v.to<double>(d) and not Yuni::Math::Equals(d, xcastData.capacity))
        {
            xcastData.capacity = d;
            if (&area == pNotifier->lastArea())
                pNotifier->reloadLastArea();
            return true;
        }
        break;
    }
    case 1:
    {
        auto d = Data::XCast::StringToDistribution(v);
        if (d != Data::XCast::dtNone and d != xcastData.distribution)
        {
            xcastData.distribution = d;
            if (&area == pNotifier->lastArea())
                pNotifier->reloadLastArea();
            return true;
        }
        break;
    }
    case 2:
    {
        bool b = v.to<bool>();
        if (b != xcastData.useConversion)
        {
            xcastData.useConversion = b;
            if (&area == pNotifier->lastArea())
                pNotifier->reloadLastArea();
            return true;
        }
        break;
    }
    case 3:
    {
        auto t = Data::XCast::CStringToTSTranslationUse(v);
        if (t != xcastData.useTranslation)
        {
            xcastData.useTranslation = t;
            if (&area == pNotifier->lastArea())
                pNotifier->reloadLastArea();
            return true;
        }
        break;
    }
    }
    return false;
}

template<enum Data::TimeSeries T>
void XCastAllAreas<T>::onAreaRenamed(Data::Area*)
{
    if (pControl)
        pControl->Refresh();
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_ALL_AREAS_HXX__
