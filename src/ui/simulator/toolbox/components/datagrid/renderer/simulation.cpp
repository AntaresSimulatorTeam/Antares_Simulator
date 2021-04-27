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

#include "simulation.h"
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
                                            Data::timeSeriesWind,
                                            Data::timeSeriesSolar};
}

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
SimulationTSManagement::SimulationTSManagement() : pControl(nullptr)
{
    // if (notifier)
    //	notifier->onSimulationTSManagementChanged.connect(this,
    //&SimulationTSManagement::onSimulationTSManagementChanged);
}

SimulationTSManagement::~SimulationTSManagement()
{
    destroyBoundEvents();
}

wxString SimulationTSManagement::columnCaption(int colIndx) const
{
    static const wxChar* const captions[] = {wxT("      Load      "),
                                             wxT("   Thermal   "),
                                             wxT("      Hydro      "),
                                             wxT("      Wind      "),
                                             wxT("      Solar      ")};
    if (colIndx < 5)
        return captions[colIndx];
    return wxEmptyString;
}

wxString SimulationTSManagement::rowCaption(int rowIndx) const
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

bool SimulationTSManagement::cellValue(int x, int y, const String& value)
{
    if (not study || x < 0 || x > 4)
        return 0.;
    auto ts = mapping[x];

    double d;
    bool conversionValid = value.to(d);
    if (!conversionValid)
    {
        bool b;
        if (value.to(b))
        {
            conversionValid = true;
            d = (b) ? 1. : 0.;
        }
    }

    switch (y)
    {
    case 1:
    {
        if (conversionValid)
        {
            if (!Math::Zero(d))
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
        if (conversionValid)
        {
            if (Math::Zero(d))
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
        if (!conversionValid)
            break;
        uint c = (uint)Math::Round(d);
        if (!c)
            c = 1;
        else
        {
            if (c > 1000)
            {
                logs.debug() << " Number of timeseries hard limit to 1000";
                c = 1000;
            }
        }
        switch (x)
        {
        case 0:
            study->parameters.nbTimeSeriesLoad = c;
            return true;
        case 1:
            study->parameters.nbTimeSeriesThermal = c;
            return true;
        case 2:
            study->parameters.nbTimeSeriesHydro = c;
            return true;
        case 3:
            study->parameters.nbTimeSeriesWind = c;
            return true;
        case 4:
            study->parameters.nbTimeSeriesSolar = c;
            return true;
        }
        onSimulationTSManagementChanged();
        break;
    }
    case 5:
    {
        if (conversionValid)
        {
            if (Math::Zero(d))
                study->parameters.timeSeriesToRefresh &= ~ts;
            else
                study->parameters.timeSeriesToRefresh |= ts;
            return true;
        }
        break;
    }
    case 6:
    {
        if (!conversionValid)
            break;
        uint refreshSpan = std::max((int)std::round(d), 1);
        switch (x)
        {
        case 0:
            study->parameters.refreshIntervalLoad = refreshSpan;
            return true;
        case 1:
            study->parameters.refreshIntervalThermal = refreshSpan;
            return true;
        case 2:
            study->parameters.refreshIntervalHydro = refreshSpan;
            return true;
        case 3:
            study->parameters.refreshIntervalWind = refreshSpan;
            return true;
        case 4:
            study->parameters.refreshIntervalSolar = refreshSpan;
            return true;
        }
        break;
    }
    case 7:
    {
        Antares::Data::Correlation::Mode mode = Data::Correlation::modeNone;
        CString<64, false> s = value;
        s.trim(" \t");
        s.toLower();
        if ((conversionValid && Math::Equals(d, +1.)) || s == "annual" || s == "a")
            mode = Data::Correlation::modeAnnual;
        else
        {
            if ((conversionValid && Math::Equals(d, -1.)) || s == "monthly" || s == "month"
                || s == "m")
                mode = Data::Correlation::modeMonthly;
        }
        if (mode != Antares::Data::Correlation::modeNone)
        {
            switch (ts)
            {
            case Data::timeSeriesLoad:
                study->preproLoadCorrelation.mode(mode);
                return true;
            case Data::timeSeriesWind:
                study->preproWindCorrelation.mode(mode);
                return true;
            case Data::timeSeriesSolar:
                study->preproSolarCorrelation.mode(mode);
                return true;
            default:
                return true;
            }
        }
        break;
    }
    case 8:
    {
        if (conversionValid)
        {
            if (Math::Zero(d))
                study->parameters.timeSeriesToImport &= ~ts;
            else
                study->parameters.timeSeriesToImport |= ts;
            return true;
        }
        break;
    }
    case 9:
    {
        if (conversionValid)
        {
            if (Math::Zero(d))
                study->parameters.timeSeriesToArchive &= ~ts;
            else
                study->parameters.timeSeriesToArchive |= ts;
            return true;
        }
        break;
    }
    case 11:
    {
        if (conversionValid)
        {
            if (Math::Zero(d))
                study->parameters.intraModal &= ~ts;
            else
                study->parameters.intraModal |= ts;
            return true;
        }
        break;
    }
    case 12:
    {
        if (conversionValid)
        {
            if (Math::Zero(d))
                study->parameters.interModal &= ~ts;
            else
                study->parameters.interModal |= ts;
            return true;
        }
        break;
    }
    }

    return false;
}

double SimulationTSManagement::cellNumericValue(int x, int y) const
{
    if (not study || x < 0 || x > 4)
        return 0.;
    auto ts = mapping[x];
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
        case 3:
            return study->parameters.nbTimeSeriesWind;
        case 4:
            return study->parameters.nbTimeSeriesSolar;
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
        case 3:
            return study->parameters.refreshIntervalWind;
        case 4:
            return study->parameters.refreshIntervalSolar;
        }
        break;
    }
    case 7:
    {
        // modeNone
        // modeAnnual
        // modeMonthly
        Data::Correlation::Mode mode = Data::Correlation::modeNone;
        switch (ts)
        {
        case Data::timeSeriesLoad:
            mode = study->preproLoadCorrelation.mode();
            break;
        case Data::timeSeriesWind:
            mode = study->preproWindCorrelation.mode();
            break;
        case Data::timeSeriesSolar:
            mode = study->preproSolarCorrelation.mode();
            break;
        default:
            return 0.;
            break;
        }
        return (mode == Data::Correlation::modeAnnual) ? 1. : -1.;
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
    return 0.;
}

wxString SimulationTSManagement::cellValue(int x, int y) const
{
    if (not study || x < 0 || x > 4)
        return wxEmptyString;
    auto ts = mapping[x];
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
        case 3:
            return wxString() << study->parameters.nbTimeSeriesWind;
        case 4:
            return wxString() << study->parameters.nbTimeSeriesSolar;
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
        case 3:
            return wxString() << study->parameters.refreshIntervalWind;
        case 4:
            return wxString() << study->parameters.refreshIntervalSolar;
        }
        break;
    }
    case 7:
    {
        // modeNone
        // modeAnnual
        // modeMonthly
        Data::Correlation::Mode mode = Data::Correlation::modeNone;
        switch (ts)
        {
        case Data::timeSeriesLoad:
            mode = study->preproLoadCorrelation.mode();
            break;
        case Data::timeSeriesWind:
            mode = study->preproWindCorrelation.mode();
            break;
        case Data::timeSeriesSolar:
            mode = study->preproSolarCorrelation.mode();
            break;
        case Data::timeSeriesHydro:
            return wxT("annual");
        case Data::timeSeriesThermal:
            return wxT("n/a");
        default:
            return wxT("--");
            break;
        }
        return (mode == Data::Correlation::modeAnnual) ? wxT("annual") : wxT("monthly");
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
    return wxEmptyString;
}

void SimulationTSManagement::onSimulationTSManagementChanged()
{
    if (pControl)
    {
        pControl->InvalidateBestSize();
        pControl->Refresh();
    }
}

IRenderer::CellStyle SimulationTSManagement::cellStyle(int x, int y) const
{
    if (not study || x < 0 || x > 4)
        return IRenderer::cellStyleError;
    auto ts = mapping[x];
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

wxColour SimulationTSManagement::horizontalBorderColor(int x, int y) const
{
    if (y == 1 || y == 9)
        return Default::BorderDaySeparator();
    return IRenderer::verticalBorderColor(x, y);
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
