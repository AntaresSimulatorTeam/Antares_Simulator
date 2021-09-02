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

#include "ts-management-clusters-as-renewables.h"
#include <wx/panel.h>
#include <yuni/core/math.h>
#include <algorithm>
#include <math.h>

using namespace Yuni;

// Anonymous namespace: global variable, local scope
namespace
{
enum Antares::Data::TimeSeries mapping[] = {Data::timeSeriesLoad,
                                            Data::timeSeriesThermal,
                                            Data::timeSeriesHydro,
                                            Data::timeSeriesRenewable};
}

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
TSmanagementRenewableCluster::TSmanagementRenewableCluster() : TSmanagement()
{}

TSmanagementRenewableCluster::~TSmanagementRenewableCluster()
{}

wxString TSmanagementRenewableCluster::columnCaption(int colIndx) const
{
    static const wxChar* const captions[] = {wxT("      Load      "),
                                             wxT("   Thermal   "),
                                             wxT("      Hydro      "),
                                             wxT("   Renewable   ")};
    if (colIndx < 4)
        return captions[colIndx];
    return wxEmptyString;
}

Antares::Data::TimeSeries TSmanagementRenewableCluster::getTSfromColumn(int col) const
{
    return mapping[col];
}

bool TSmanagementRenewableCluster::cellValueForRenewables(int x, int y, const double v) const
{
    // Does nothing
    return true;
}

/*
double TSmanagementRenewableCluster::cellNumericValue(int x, int y) const
{
    // Function never called, but has to overload a pure virtual function.
    return 0.;
}
*/

double TSmanagementRenewableCluster::cellNumericValueForRenewables(int x, int y) const
{
    return 0.;
}

wxString TSmanagementRenewableCluster::cellValueForRenewables(int x, int y) const
{
    switch (y)
    {
    case 0:
        return wxEmptyString;
    case 1:
        return wxT("On");
    case 11:
        return (0 != (study->parameters.intraModal & Data::timeSeriesRenewable)) ? wxT("Yes") : wxT("No");
    case 12:
        return (0 != (study->parameters.interModal & Data::timeSeriesRenewable)) ? wxT("Yes") : wxT("No");
    }

    return wxT("-");
}

void TSmanagementRenewableCluster::onSimulationTSManagementChanged()
{
    if (pControl)
    {
        pControl->InvalidateBestSize();
        pControl->Refresh();
    }
}

IRenderer::CellStyle TSmanagementRenewableCluster::cellStyleForRenewables(int x, int y) const
{
    switch (y)
    {
    case 0:
        return IRenderer::cellStyleDefault;
    case 11:
    {
        return (0 != (study->parameters.intraModal & Data::timeSeriesRenewable)) ? IRenderer::cellStyleDefault
            : IRenderer::cellStyleDefaultDisabled;
    }
    case 12:
    {
        return (0 != (study->parameters.interModal & Data::timeSeriesRenewable)) ? IRenderer::cellStyleDefault
            : IRenderer::cellStyleDefaultDisabled;
    }
    }
    return CellStyle::cellStyleDisabled;
}

IRenderer::CellStyle TSmanagementRenewableCluster::cellStyle(int x, int y) const
{
    if (not study || x < 0 || x > 3)
        return IRenderer::cellStyleError;
    auto ts = mapping[x];

    // Renewable clusters only
    if (ts == Data::timeSeriesRenewable)
        return cellStyleForRenewables(x, y);

    bool tsGenerator = (0 != (study->parameters.timeSeriesToGenerate & ts));

    switch (y)
    {
    case 0:
    case 2:
        break;
    case 1:
    {
        // Status READY made TS
        return tsGenerator ? IRenderer::cellStyleConstraintNoWeight
                           : IRenderer::cellStyleConstraintWeight;
    }
    case 3:
    {
        // Status Stochastic made TS
        return (!tsGenerator) ? IRenderer::cellStyleConstraintNoWeight
                              : IRenderer::cellStyleConstraintWeight;
    }
    case 6:
    {
        return (tsGenerator && 0 != (study->parameters.timeSeriesToRefresh & ts))
                 ? IRenderer::cellStyleDefault
                 : IRenderer::cellStyleDefaultDisabled;
    }
    case 7:
    {
        return (tsGenerator) ? IRenderer::cellStyleDefault : IRenderer::cellStyleDefaultDisabled;
    }
    case 8:
    {
        return (tsGenerator && 0 != (study->parameters.timeSeriesToImport & ts))
                 ? IRenderer::cellStyleDefault
                 : IRenderer::cellStyleDefaultDisabled;
    }
    case 9:
    {
        return (tsGenerator && 0 != (study->parameters.timeSeriesToArchive & ts))
                 ? IRenderer::cellStyleDefault
                 : IRenderer::cellStyleDefaultDisabled;
    }
    case 11:
    {
        return (0 != (study->parameters.intraModal & ts)) ? IRenderer::cellStyleDefault
                                                          : IRenderer::cellStyleDefaultDisabled;
    }
    case 12:
    {
        return (0 != (study->parameters.interModal & ts)) ? IRenderer::cellStyleDefault
                                                          : IRenderer::cellStyleDefaultDisabled;
    }
    }

    // default style
    return tsGenerator ? IRenderer::cellStyleDefault : IRenderer::cellStyleDefaultDisabled;
}

wxColour TSmanagementRenewableCluster::horizontalBorderColor(int x, int y) const
{
    if (y == 1 || y == 9)
        return Default::BorderDaySeparator();
    return IRenderer::verticalBorderColor(x, y);
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
