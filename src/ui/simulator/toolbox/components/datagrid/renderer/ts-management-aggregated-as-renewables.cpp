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

#include "ts-management-aggregated-as-renewables.h"

// Anonymous namespace: global variable, local scope
namespace
{
    enum Antares::Data::TimeSeries mapping[] = { Data::timeSeriesLoad,
                                                 Data::timeSeriesThermal,
                                                 Data::timeSeriesHydro,
                                                 Data::timeSeriesWind,
                                                 Data::timeSeriesSolar};
}

TSmanagementRenewableAggregated::TSmanagementRenewableAggregated() : TSmanagement()
{
}

TSmanagementRenewableAggregated::~TSmanagementRenewableAggregated()
{}

wxString TSmanagementRenewableAggregated::columnCaption(int colIndx) const
{
    static const wxChar* const captions[] = { wxT("      Load      "),
                                              wxT("   Thermal   "),
                                              wxT("      Hydro      "),
                                              wxT("      Wind      "),
                                              wxT("      Solar      ") };
    if (colIndx < 5) // gp : 5 -> width() function 
        return captions[colIndx];
    return wxEmptyString;
}

Antares::Data::TimeSeries TSmanagementRenewableAggregated::getTSfromColumn(int col) const
{
    return mapping[col];
}

bool TSmanagementRenewableAggregated::cellValueForRenewables(int x, int y, const double v) const
{
    auto ts = getTSfromColumn(x);

    switch (y)
    {
    case 4:
    {
        if (!pConversionToDoubleValid)
            break;
        switch (x)
        {
        case 3:
            study->parameters.nbTimeSeriesWind = pNumberTS;
            return true;
        case 4:
            study->parameters.nbTimeSeriesSolar = pNumberTS;
            return true;
        }
        onSimulationTSManagementChanged();
        break;
    }
    case 6:
    {
        if (!pConversionToDoubleValid)
            break;
        switch (x)
        {
        case 3:
            study->parameters.refreshIntervalWind = pRefreshSpan;
            return true;
        case 4:
            study->parameters.refreshIntervalSolar = pRefreshSpan;
            return true;
        }
        break;
    }
    case 7:
    {
        if (pMode != Antares::Data::Correlation::modeNone)
        {
            switch (ts)
            {
            case Data::timeSeriesWind:
                study->preproWindCorrelation.mode(pMode);
                return true;
            case Data::timeSeriesSolar:
                study->preproSolarCorrelation.mode(pMode);
                return true;
            default:
                return true;
            }
        }
        break;
    }
    }

    return false;
}

double TSmanagementRenewableAggregated::cellNumericValueForRenewables(int x, int y) const
{
    auto ts = getTSfromColumn(x);

    switch (y)
    {
    case 4:
    {
        switch (x)
        {
        case 3:
            return study->parameters.nbTimeSeriesWind;
        case 4:
            return study->parameters.nbTimeSeriesSolar;
        }
        break;
    }
    case 6:
    {
        switch (x)
        {
        case 3:
            return study->parameters.refreshIntervalWind;
        case 4:
            return study->parameters.refreshIntervalSolar;
        }
        break;
    }

    case 7:
    {
        switch (ts)
        {
        case Data::timeSeriesWind:
            pMode = study->preproWindCorrelation.mode();
            break;
        case Data::timeSeriesSolar:
            pMode = study->preproSolarCorrelation.mode();
            break;
        default:
            return 0.;
            break;
        }
        return (pMode == Data::Correlation::modeAnnual) ? 1. : -1.;
    }
    }

    return 0.;
}

wxString TSmanagementRenewableAggregated::cellValueForRenewables(int x, int y) const
{
    auto ts = getTSfromColumn(x);

    case 4:
    {
        switch (x)
        {
        case 3:
            return wxString() << study->parameters.nbTimeSeriesWind;
        case 4:
            return wxString() << study->parameters.nbTimeSeriesSolar;
        }
        break;
    }

    case 6:
    {
        switch (x)
        {
        case 3:
            return wxString() << study->parameters.refreshIntervalWind;
        case 4:
            return wxString() << study->parameters.refreshIntervalSolar;
        }
        break;
    }
    case 7:
    {
        switch (ts)
        {
        case Data::timeSeriesWind:
            pMode = study->preproWindCorrelation.mode();
            break;
        case Data::timeSeriesSolar:
            pMode = study->preproSolarCorrelation.mode();
            break;
        default:
            return wxT("--");
            break;
        }
        return (pMode == Data::Correlation::modeAnnual) ? wxT("annual") : wxT("monthly");
    }
}