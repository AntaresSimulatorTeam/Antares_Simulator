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

#include "ts-management.h"
#include <wx/panel.h>
#include <yuni/core/math.h>
#include <algorithm>
#include <math.h>

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{

bool convertValueToDouble(const String& value, double& returned)
{
    bool isConversionValid = value.to(returned);
    if (!isConversionValid)
    {
        bool b;
        if (value.to(b))
        {
            isConversionValid = true;
            returned = (b) ? 1. : 0.;
        }
    }
    return isConversionValid;
}

Antares::Data::Correlation::Mode convertValueToCorrelationMode(const String& value, double valueToDouble, bool conversionToDoubleValid)
{
    Antares::Data::Correlation::Mode modeToReturn = Data::Correlation::modeNone;
    CString<64, false> s = value;
    s.trim(" \t");
    s.toLower();
    if ((conversionToDoubleValid && Math::Equals(valueToDouble, +1.)) || s == "annual" || s == "a")
        modeToReturn = Data::Correlation::modeAnnual;
    else
    {
        if ((conversionToDoubleValid && Math::Equals(valueToDouble, -1.)) || s == "monthly" || s == "month"
            || s == "m")
            modeToReturn = Data::Correlation::modeMonthly;
    }
    return modeToReturn;
}

TSmanagement::TSmanagement() : pControl(nullptr)
{
    pConversionToDoubleValid = true;
    pNumberTS = 0;
    pRefreshSpan = 0;
    pValueToDouble = -1;
    pMode = Data::Correlation::modeNone;
}


TSmanagement::~TSmanagement()
{
    destroyBoundEvents();
}

wxString TSmanagement::rowCaption(int rowIndx) const
{
    static const wxChar* const captions[] = {
      wxT("Ready made TS"),
      wxT("        Status"),
      wxT("Stochastic TS"),
      wxT("        Status"),
      wxT("        Number"),
      wxT("        Refresh"),
      wxT("        Refresh span"),
      wxT("        Seasonal correlation "),
      wxT("        Store in input  "),
      wxT("        Store in output  "),
      wxT("Draws correlation  "),
      wxT("        intra-modal"),
      wxT("        inter-modal"),
    };
    if (rowIndx < 13)
        return captions[rowIndx];
    return wxEmptyString;
}

bool TSmanagement::cellValue(int x, int y, const String& value)
{
    if (not study || x < 0 || x > width())
        return 0.;
    auto ts = getTSfromColumn(x);

    pConversionToDoubleValid = convertValueToDouble(value, pValueToDouble);

    switch (y)
    {
    case 1:
    {
        if (pConversionToDoubleValid)
        {
            if (!Math::Zero(pValueToDouble))
                study->parameters.timeSeriesToGenerate &= ~ts;
            else
                study->parameters.timeSeriesToGenerate |= ts;
            onSimulationTSManagementChanged();
            return true;
        }
        break;
    }
    case 3:
    {
        if (pConversionToDoubleValid)
        {
            if (Math::Zero(pValueToDouble))
                study->parameters.timeSeriesToGenerate &= ~ts;
            else
                study->parameters.timeSeriesToGenerate |= ts;
            onSimulationTSManagementChanged();
            return true;
        }
        break;
    }
    case 4:
    {
        if (!pConversionToDoubleValid)
            break;
        pNumberTS = (uint)Math::Round(pValueToDouble);
        if (!pNumberTS)
            pNumberTS = 1;
        else
        {
            if (pNumberTS > 1000)
            {
                logs.debug() << " Number of timeseries hard limit to 1000";
                pNumberTS = 1000;
            }
        }
        switch (x)
        {
        case 0:
            study->parameters.nbTimeSeriesLoad = pNumberTS;
            return true;
        case 1:
            study->parameters.nbTimeSeriesThermal = pNumberTS;
            return true;
        case 2:
            study->parameters.nbTimeSeriesHydro = pNumberTS;
            return true;
        }
        onSimulationTSManagementChanged();
        break;
    }
    case 5:
    {
        if (pConversionToDoubleValid)
        {
            if (Math::Zero(pValueToDouble))
                study->parameters.timeSeriesToRefresh &= ~ts;
            else
                study->parameters.timeSeriesToRefresh |= ts;
            return true;
        }
        break;
    }
    case 6:
    {
        if (!pConversionToDoubleValid)
            break;
        pRefreshSpan = std::max((int)std::round(pValueToDouble), 1);
        switch (x)
        {
        case 0:
            study->parameters.refreshIntervalLoad = pRefreshSpan;
            return true;
        case 1:
            study->parameters.refreshIntervalThermal = pRefreshSpan;
            return true;
        case 2:
            study->parameters.refreshIntervalHydro = pRefreshSpan;
            return true;
        }
        break;
    }
    case 7:
    {
        pMode = convertValueToCorrelationMode(value, pValueToDouble, pConversionToDoubleValid);

        if (pMode != Antares::Data::Correlation::modeNone)
        {
            switch (ts)
            {
            case Data::timeSeriesLoad:
                study->preproLoadCorrelation.mode(pMode);
                return true;
            }
        }
        break;
    }
    case 8:
    {
        if (pConversionToDoubleValid)
        {
            if (Math::Zero(pValueToDouble))
                study->parameters.timeSeriesToImport &= ~ts;
            else
                study->parameters.timeSeriesToImport |= ts;
            return true;
        }
        break;
    }
    case 9:
    {
        if (pConversionToDoubleValid)
        {
            if (Math::Zero(pValueToDouble))
                study->parameters.timeSeriesToArchive &= ~ts;
            else
                study->parameters.timeSeriesToArchive |= ts;
            return true;
        }
        break;
    }
    case 11:
    {
        if (pConversionToDoubleValid)
        {
            if (Math::Zero(pValueToDouble))
                study->parameters.intraModal &= ~ts;
            else
                study->parameters.intraModal |= ts;
            return true;
        }
        break;
    }
    case 12:
    {
        if (pConversionToDoubleValid)
        {
            if (Math::Zero(pValueToDouble))
                study->parameters.interModal &= ~ts;
            else
                study->parameters.interModal |= ts;
            return true;
        }
        break;
    }
    }

    return cellValueForRenewables(x, y, pConversionToDoubleValid);
}

double TSmanagement::cellNumericValue(int x, int y)
{
    if (not study || x < 0 || x > width())
        return 0.;
    auto ts = getTSfromColumn(x);
    switch (y)
    {
    case 0:
    case 2:
        break;
    case 1:
    {
        // Status READY made TS
        return (0 != (study->parameters.timeSeriesToGenerate & ts)) ? 0 : 1.;
    }
    case 3:
    {
        // Status Stochastic made TS
        return (0 != (study->parameters.timeSeriesToGenerate & ts)) ? 1. : 0.;
    }
    case 4:
    {
        switch (x)
        {
        case 0:
            return study->parameters.nbTimeSeriesLoad;
        case 1:
            return study->parameters.nbTimeSeriesThermal;
        case 2:
            return study->parameters.nbTimeSeriesHydro;
        }
        break;
    }
    case 5:
    {
        return (0 != (study->parameters.timeSeriesToRefresh & ts)) ? 1. : 0.;
    }
    case 6:
    {
        switch (x)
        {
        case 0:
            return study->parameters.refreshIntervalLoad;
        case 1:
            return study->parameters.refreshIntervalThermal;
        case 2:
            return study->parameters.refreshIntervalHydro;
        }
        break;
    }
    case 7:
    {
        // modeNone
        // modeAnnual
        // modeMonthly
        pMode = Data::Correlation::modeNone;
        switch (ts)
        {
        case Data::timeSeriesLoad:
            pMode = study->preproLoadCorrelation.mode();
            break;
        }
    }
    case 8:
    {
        return (0 != (study->parameters.timeSeriesToImport & ts)) ? 1. : 0.;
    }
    case 9:
    {
        return (0 != (study->parameters.timeSeriesToArchive & ts)) ? 1. : 0.;
    }
    case 11:
    {
        return (0 != (study->parameters.intraModal & ts)) ? 1. : 0.;
    }
    case 12:
    {
        return 0.;
    }
    }

    return cellNumericValueForRenewables(x, y);
}

wxString TSmanagement::cellValue(int x, int y)
{
    if (not study || x < 0 || x > width())
        return wxEmptyString;
    auto ts = getTSfromColumn(x);
    switch (y)
    {
    case 1:
    {
        // Status READY made TS
        return (0 != (study->parameters.timeSeriesToGenerate & ts)) ? wxT("Off") : wxT("On");
    }
    case 3:
    {
        // Status Stochastic made TS
        return (0 != (study->parameters.timeSeriesToGenerate & ts)) ? wxT("On") : wxT("Off");
    }
    case 4:
    {
        switch (x)
        {
        case 0:
            return wxString() << study->parameters.nbTimeSeriesLoad;
        case 1:
            return wxString() << study->parameters.nbTimeSeriesThermal;
        case 2:
            return wxString() << study->parameters.nbTimeSeriesHydro;
        }
        break;
    }
    case 5:
        return (0 != (study->parameters.timeSeriesToRefresh & ts)) ? wxT("Yes") : wxT("No");
    case 6:
    {
        switch (x)
        {
        case 0:
            return wxString() << study->parameters.refreshIntervalLoad;
        case 1:
            return wxString() << study->parameters.refreshIntervalThermal;
        case 2:
            return wxString() << study->parameters.refreshIntervalHydro;
        }
        break;
    }
    case 7:
    {
        // modeNone
        // modeAnnual
        // modeMonthly
        pMode = Data::Correlation::modeNone;
        switch (ts)
        {
        case Data::timeSeriesLoad:
            pMode = study->preproLoadCorrelation.mode();
            break;
        case Data::timeSeriesHydro:
            return wxT("annual");
        case Data::timeSeriesThermal:
            return wxT("n/a");
        default:
            break;
        }
    }
    case 8:
        return (0 != (study->parameters.timeSeriesToImport & ts)) ? wxT("Yes") : wxT("No");
    case 9:
        return (0 != (study->parameters.timeSeriesToArchive & ts)) ? wxT("Yes") : wxT("No");
    case 11:
        return (0 != (study->parameters.intraModal & ts)) ? wxT("Yes") : wxT("No");
    case 12:
        return (0 != (study->parameters.interModal & ts)) ? wxT("Yes") : wxT("No");
    }

    return cellValueForRenewables(x, y);
    // return wxEmptyString;
}

void TSmanagement::onSimulationTSManagementChanged()
{
    if (pControl)
    {
        pControl->InvalidateBestSize();
        pControl->Refresh();
    }
}

IRenderer::CellStyle TSmanagement::cellStyle(int x, int y) const
{
    if (not study || x < 0 || x > 5)
        return IRenderer::cellStyleError;
    auto ts = getTSfromColumn(x);
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

wxColour TSmanagement::horizontalBorderColor(int x, int y) const
{
    if (y == 1 || y == 9)
        return Default::BorderDaySeparator();
    return IRenderer::verticalBorderColor(x, y);
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
