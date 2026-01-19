// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "summary.h"
#include <yuni/core/math.h>
#include <wx/window.h>
#include "antares/study/ui-runtimeinfos.h"

using namespace Yuni;

namespace Antares::Component::Datagrid::Renderer::Links
{
Summary::Summary(wxWindow* control):
    pControl(control)
{
}

Summary::~Summary()
{
    destroyBoundEvents();
}

wxString Summary::rowCaption(int rowIndx) const
{
    auto study = GetCurrentStudy();
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
    auto study = GetCurrentStudy();
    if (!study)
    {
        return 0;
    }

    assert(study->uiinfo);
    auto& uiinfo = *(study->uiinfo);
    Data::AreaLink* link = (uint)y < uiinfo.linkCount() ? uiinfo.link((uint)y) : nullptr;
    if (!link)
    {
        return 0.;
    }
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
    if (not CurrentStudyIsValid())
    {
        return wxEmptyString;
    }
    assert(GetCurrentStudy()->uiinfo);
    auto& uiinfo = *(GetCurrentStudy()->uiinfo);
    auto* link = (uint)y < uiinfo.linkCount() ? uiinfo.link((uint)y) : nullptr;
    if (not link)
    {
        return wxEmptyString;
    }
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
    if (!CurrentStudyIsValid())
    {
        return IRenderer::cellStyleDefault;
    }
    assert(GetCurrentStudy()->uiinfo);
    auto& uiinfo = *(GetCurrentStudy()->uiinfo);
    Data::AreaLink* link = (uint)y < uiinfo.linkCount() ? uiinfo.link((uint)y) : nullptr;
    if (!link)
    {
        return IRenderer::cellStyleDefault;
    }

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
    if (!CurrentStudyIsValid())
    {
        return false;
    }
    assert(GetCurrentStudy()->uiinfo);
    auto& uiinfo = *(GetCurrentStudy()->uiinfo);
    auto* link = (uint)y < uiinfo.linkCount() ? uiinfo.link((uint)y) : nullptr;
    if (not link)
    {
        return false;
    }

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
        {
            link->transmissionCapacities = LocalTransmissionCapacities::enabled;
        }
        else if (pTmp == "infinite" || pTmp == "set to infinite" || pTmp == "inf" || pTmp == "i")
        {
            link->transmissionCapacities = LocalTransmissionCapacities::infinite;
        }
        else if (pTmp == "0" || pTmp == "zero" || pTmp == "null" || pTmp == "set to null"
                 || pTmp == "n")
        {
            link->transmissionCapacities = LocalTransmissionCapacities::null;
        }

        OnInspectorRefresh(nullptr);
        MarkTheStudyAsModified();
        return true;
    }
    return false;
}

int Summary::height() const
{
    auto study = GetCurrentStudy();
    return !study ? 0 : study->uiinfo->linkCount();
}

} // namespace Antares::Component::Datagrid::Renderer::Links
