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

#include "renewable.areasummary.h"
#include "../../../refresh.h"
#include "../../../../../application/study.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
RenewableClusterSummarySingleArea::RenewableClusterSummarySingleArea(
  wxWindow* control,
  Toolbox::InputSelector::Area* notifier) :
 pArea(nullptr), pControl(control), pAreaNotifier(notifier)
{
    if (notifier)
        notifier->onAreaChanged.connect(this, &RenewableClusterSummarySingleArea::onAreaChanged);
}

RenewableClusterSummarySingleArea::~RenewableClusterSummarySingleArea()
{
    destroyBoundEvents();
}

wxString RenewableClusterSummarySingleArea::rowCaption(int rowIndx) const
{
    // gp : causes a crash when saving a study
    // if (pArea)
    //     return wxStringFromUTF8(pArea->renewable.list.byIndex[rowIndx]->name());
    return wxEmptyString;
}

wxString RenewableClusterSummarySingleArea::columnCaption(int colIndx) const
{
    // note: Those ugly white-spaces are required (for now)
    //  to fix the min width of the column. A method should be added for this purpose.
    static const wxChar* const rows[] = {
      wxT("          Group          "),
      wxT("  Enabled  "),
      wxT("  Unit count  "),
      wxT("   Nom. Capacity  \n        MW        "),
    };
    return (colIndx < 5 and colIndx >= 0) ? rows[colIndx] : wxT("");
}

wxString RenewableClusterSummarySingleArea::cellValue(int x, int y) const
{
    /*
    Data::RenewableCluster* cluster = (pArea and (uint) y < pArea->renewable.list.size())
                                      ? pArea->renewable.list.byIndex[y]
                                      : nullptr;
    if (!cluster->enabled)
        return wxEmptyString;
    switch (x)
    {
    case 0:
        return wxStringFromUTF8(cluster->group());
    case 1:
        return cluster->enabled ? wxT("Yes") : wxT("no");
    case 2:
        return DoubleToWxString(cluster->nominalCapacity);
    }
    */
    return wxEmptyString;
}

double RenewableClusterSummarySingleArea::cellNumericValue(int x, int y) const
{
    /*
    Data::RenewableCluster* cluster = (pArea and (uint) y < pArea->renewable.list.size())
                                      ? pArea->renewable.list.byIndex[y]
                                      : nullptr;
    if (!cluster->enabled)
        return 0.;
    switch (x)
    {
    case 0:
        return 0.;
    case 1:
        return cluster->enabled ? 1. : 0.;
    case 3:
        return cluster->nominalCapacity;
    }
    */
    return 0.;
}

void RenewableClusterSummarySingleArea::onAreaChanged(Antares::Data::Area* area)
{
    /*
    if (pArea != area)
    {
        pArea = area;
        RefreshAllControls(pControl);
    }
    */
}

IRenderer::CellStyle RenewableClusterSummarySingleArea::cellStyle(int col, int row) const
{
    return (col > 0 and Math::Zero(cellNumericValue(col, row)))
             ? IRenderer::cellStyleDefaultDisabled
             : (col == 1 || col == 2) ? IRenderer::cellStyleConstraintWeight
                                      : IRenderer::cellStyleDefault;
}

struct NoCheck
{
    template<class T>
    static bool Validate(const T&)
    {
        return true;
    }
};

struct CheckMinUpDownTime
{
    static bool Validate(uint f)
    {
        return (f == 1) || (f == 24) || (f == 168);
    }
};

struct CheckUnitCount
{
    static bool Validate(uint& f)
    {
        if (f > 100)
            f = 100;
        return true;
    }
};

template<class CheckT>
static bool UpdateUnsignedLong(uint& value, const String& str)
{
    uint l;
    if (str.to(l))
    {
        if (value != l and CheckT::Validate(l))
        {
            value = l;
            MarkTheStudyAsModified();
            OnInspectorRefresh(nullptr);
            return true;
        }
    }
    return false;
}

template<class CheckT>
static bool UpdateDouble(double& value, const String& str)
{
    double d;
    if (str.to(d))
    {
        if (not Math::Equals<double>(value, d))
        {
            if (CheckT::Validate(d))
            {
                value = d;
                OnInspectorRefresh(nullptr);
                return true;
            }
        }
    }
    return false;
}

template<class CheckT>
static bool UpdateBool(bool& value, const String& str)
{
    bool d;
    if (str.to(d))
    {
        if (value != d)
        {
            if (CheckT::Validate(d))
            {
                value = d;
                OnInspectorRefresh(nullptr);
                return true;
            }
        }
    }
    return false;
}

bool RenewableClusterSummarySingleArea::cellValue(int x, int y, const String& v)
{
    auto* cluster = (pArea and (uint) y < pArea->renewable.list.size())
                      ? pArea->renewable.list.byIndex[y]
                      : nullptr;

    if (cluster)
    {
        switch (x)
        {
        case 0:
        {
            if (cluster->group() != v)
            {
                cluster->group(v);
                return true;
            }
            break;
        }
        case 1:
            return UpdateBool<NoCheck>(cluster->enabled, v);
        case 2:
            return UpdateDouble<NoCheck>(cluster->nominalCapacity, v);
        }
    }
    return false;
}

void RenewableClusterSummarySingleArea::onStudyClosed()
{
    pArea = nullptr;
    IRenderer::onStudyClosed();
}

void RenewableClusterSummarySingleArea::onStudyAreaDelete(Antares::Data::Area* area)
{
    if (pArea == area)
        onAreaChanged(nullptr);
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
