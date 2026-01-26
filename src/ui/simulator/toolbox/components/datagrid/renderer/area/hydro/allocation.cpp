// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "allocation.h"
#include "../../../../../../application/study.h"
#include <yuni/core/math.h>

using namespace Yuni;

namespace Antares::Component::Datagrid::Renderer
{
HydroAllocation::HydroAllocation()
{
}

HydroAllocation::~HydroAllocation()
{
}

bool HydroAllocation::valid() const
{
    return CurrentStudyIsValid();
}

int HydroAllocation::width() const
{
    auto study = GetCurrentStudy();
    return !(!study) ? study->areas.size() : 0;
}

int HydroAllocation::height() const
{
    auto study = GetCurrentStudy();
    return !(!study) ? study->areas.size() : 0;
}

wxString HydroAllocation::columnCaption(int colIndx) const
{
    auto study = GetCurrentStudy();
    return (study && (uint)colIndx < study->areas.size())
             ? wxStringFromUTF8(" hydro \n " + study->areas[colIndx]->name)
             : wxString(); // MBO 21/05/2014 - #22
}

wxString HydroAllocation::rowCaption(int rowIndx) const
{
    auto study = GetCurrentStudy();
    return (study && (uint)rowIndx < study->areas.size())
             ? wxStringFromUTF8("load " + study->areas[rowIndx]->name)
             : wxString(); // MBO 21/05/2014 - #22
}

wxString HydroAllocation::cellValue(int x, int y) const
{
    auto study = GetCurrentStudy();
    if (!(!study))
    {
        auto colArea = (uint)x;
        auto rowArea = (uint)y;
        auto areaCount = study->areas.size();
        if (colArea < areaCount && rowArea < areaCount)
        {
            auto& area = *(study->areas[colArea]);
            auto& target = *(study->areas[rowArea]);
            double v = area.hydro.allocation[target];
            if (!Math::Zero(v))
            {
                return DoubleToWxString(v);
            }
        }
    }
    static const wxString zero = wxT("0");
    return zero;
}

double HydroAllocation::cellNumericValue(int x, int y) const
{
    auto study = GetCurrentStudy();
    if (!(!study))
    {
        auto colArea = (uint)x;
        auto rowArea = (uint)y;
        auto areaCount = study->areas.size();
        if (colArea < areaCount && rowArea < areaCount)
        {
            auto& area = *(study->areas[colArea]);
            auto& target = *(study->areas[rowArea]);
            return area.hydro.allocation[target];
        }
    }
    return 0;
}

bool HydroAllocation::cellValue(int x, int y, const String& value)
{
    double v;
    if (!value.to(v))
    {
        return false;
    }

    auto study = GetCurrentStudy();
    if (!(!study))
    {
        auto colArea = (uint)x;
        auto rowArea = (uint)y;
        auto areaCount = study->areas.size();
        if (colArea < areaCount && rowArea < areaCount)
        {
            auto& area = *(study->areas[colArea]);
            auto& target = *(study->areas[rowArea]);
            area.hydro.allocation.fromArea(target, v);
            return true;
        }
    }
    return false;
}

IRenderer::CellStyle HydroAllocation::cellStyle(int col, int row) const
{
    if (Math::Zero(cellNumericValue(col, row)))
    {
        return !(col % 2) ? IRenderer::cellStyleDefaultCenterDisabled
                          : IRenderer::cellStyleDefaultCenterAlternateDisabled;
    }
    else
    {
        return IRenderer::cellStyleConstraintWeight;
    }
}

} // namespace Antares::Component::Datagrid::Renderer
