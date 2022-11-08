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

#include "summary.h"
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
namespace Links
{
Summary::Summary(wxWindow* control) : pControl(control)
{
}

Summary::~Summary()
{
    destroyBoundEvents();
}

wxString Summary::rowCaption(int rowIndx) const
{
    auto study = Data::Study::Current::Get();
    if (!(!study))
    {
        auto* lnk = study->uiinfo->link((uint)(rowIndx));
        assert(lnk);
        assert(lnk->from);
        assert(lnk->with);

        wxString s;
        s << wxStringFromUTF8(lnk->from->id);
        s << wxT(" / ") << wxStringFromUTF8(lnk->with->id);
        return s;
    }
    return wxEmptyString;
}

double Summary::cellNumericValue(int x, int y) const
{
    auto study = Data::Study::Current::Get();
    if (!study)
        return 0;

    assert(study->uiinfo);
    auto& uiinfo = *(study->uiinfo);
    Data::AreaLink* link = (uint)y < uiinfo.linkCount() ? uiinfo.link((uint)y) : nullptr;
    if (!link)
        return 0.;
    switch (x)
    {
    case 0:
        return (link->useHurdlesCost) ? 1. : 0.;
    case 1:
        return (link->transmissionCapacities == Data::LocalTransmissionCapacities::enabled) ? 1.
                                                                                            : 0.;
    }
    return 0.;
}

wxString Summary::cellValue(int x, int y) const
{
    if (not Data::Study::Current::Valid())
        return wxEmptyString;
    assert(Data::Study::Current::Get()->uiinfo);
    auto& uiinfo = *(Data::Study::Current::Get()->uiinfo);
    auto* link = (uint)y < uiinfo.linkCount() ? uiinfo.link((uint)y) : nullptr;
    if (not link)
        return wxEmptyString;
    switch (x)
    {
    case 0:
        return (link->useHurdlesCost) ? wxT("enabled") : wxT("ignore");
    case 1:
        switch (link->transmissionCapacities)
        {
        case Data::LocalTransmissionCapacities::enabled:
            return wxT("enabled");
        case Data::LocalTransmissionCapacities::null:
            return wxT("set to null");
        case Data::LocalTransmissionCapacities::infinite:
            return wxT("set to infinite");
        default:
            return wxEmptyString;
        }
    }
    return wxEmptyString;
}

wxString Summary::columnCaption(int x) const
{
    switch (x)
    {
    case 0:
        return wxT("  Hurdles Cost   ");
    case 1:
        return wxT("   Transmission capacities   ");
    default:
        return wxEmptyString;
    }
    return wxEmptyString;
}

IRenderer::CellStyle Summary::cellStyle(int x, int y) const
{
    using namespace Data;
    if (!Study::Current::Valid())
        return IRenderer::cellStyleDefault;
    assert(Study::Current::Get()->uiinfo);
    auto& uiinfo = *(Study::Current::Get()->uiinfo);
    Data::AreaLink* link = (uint)y < uiinfo.linkCount() ? uiinfo.link((uint)y) : nullptr;
    if (!link)
        return IRenderer::cellStyleDefault;

    switch (x)
    {
    case 0:
        return (not link->useHurdlesCost) ? IRenderer::cellStyleConstraintWeight
                                          : IRenderer::cellStyleDefault;
    case 1:
        return (link->transmissionCapacities != LocalTransmissionCapacities::enabled)
                 ? IRenderer::cellStyleConstraintWeight
                 : IRenderer::cellStyleDefault;
    }
    return IRenderer::cellStyleDefault;
}

bool Summary::cellValue(int x, int y, const String& value)
{
    using namespace Data;
    if (!Study::Current::Valid())
        return false;
    assert(Study::Current::Get()->uiinfo);
    auto& uiinfo = *(Study::Current::Get()->uiinfo);
    auto* link = (uint)y < uiinfo.linkCount() ? uiinfo.link((uint)y) : nullptr;
    if (not link)
        return false;

    CString<10, false> pTmp = value;
    pTmp.trim();
    pTmp.toLower();
    bool v = pTmp.to<bool>();

    switch (x)
    {
    case 0:
        link->useHurdlesCost = v;
        OnInspectorRefresh(nullptr);
        MarkTheStudyAsModified();
        return true;
    case 1:
        if (pTmp == "enabled" || pTmp == "e")
            link->transmissionCapacities = LocalTransmissionCapacities::enabled;
        else if (pTmp == "infinite" || pTmp == "set to infinite" || pTmp == "inf" || pTmp == "i")
            link->transmissionCapacities = LocalTransmissionCapacities::enabled;
        else if (pTmp == "0" || pTmp == "zero" || pTmp == "null" || pTmp == "set to null"
                 || pTmp == "n")
            link->transmissionCapacities = LocalTransmissionCapacities::null;

        OnInspectorRefresh(nullptr);
        MarkTheStudyAsModified();
        return true;
    }
    return false;
}

int Summary::height() const
{
    auto study = Data::Study::Current::Get();
    return !study ? 0 : study->uiinfo->linkCount();
}

} // namespace Links
} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
