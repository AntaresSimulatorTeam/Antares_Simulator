/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "timeseries.h"
#include <float.h>
#include "../../../../../application/study.h"
#include "../../default.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
template<class T1, class T2>
static double ComputeAverageOnRow(Antares::Matrix<T1, T2>* m, int r)
{
    if (m && m->width && m->height)
    {
        double ret = 0.;
        for (uint x = 0; x != m->width; ++x)
            ret += (double)m->entry[x][r];
        return ret / m->width;
    }
    return 0.;
}

template<class T1, class T2>
static double ComputeMinOnRow(Antares::Matrix<T1, T2>* m, int r)
{
    if (m && m->width && m->height)
    {
        double ret = +std::numeric_limits<double>::infinity();
        for (uint x = 0; x != m->width; ++x)
        {
            if (ret > (double)(*m)[x][r])
                ret = (double)(*m)[x][r];
        }
        return ret;
    }
    return 0.;
}

template<class T1, class T2>
static double ComputeMaxOnRow(Antares::Matrix<T1, T2>* m, int r)
{
    if (m && m->width)
    {
        double ret = -std::numeric_limits<double>::infinity();
        for (uint x = 0; x != m->width; ++x)
        {
            if (ret < (double)(*m)[x][r])
                ret = (double)(*m)[x][r];
        }
        return ret;
    }
    return 0.;
}

ATimeSeries::ATimeSeries(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
 AncestorType(control), Renderer::ARendererArea(control, notifier)
{
}

ATimeSeries::~ATimeSeries()
{
    destroyBoundEvents();
}

void ATimeSeries::onStudyClosed()
{
    AncestorType::onStudyClosed();
    Renderer::ARendererArea::onStudyClosed();
}

void ATimeSeries::onStudyLoaded()
{
    AncestorType::onStudyLoaded();
    Renderer::ARendererArea::onStudyLoaded();
}

wxString ATimeSeries::cellValue(int x, int y) const
{
    if (x < AncestorType::width())
        return AncestorType::cellValue(x, y);
    if (x == AncestorType::width())
        return DoubleToWxString(Math::Round(ComputeAverageOnRow(pMatrix, y), 2));
    if (x == AncestorType::width() + 1)
        return DoubleToWxString(ComputeMinOnRow(pMatrix, y));
    if (x == AncestorType::width() + 2)
        return DoubleToWxString(ComputeMaxOnRow(pMatrix, y));
    return wxT("0");
}

double ATimeSeries::cellNumericValue(int x, int y) const
{
    if (x < AncestorType::width())
        return AncestorType::cellNumericValue(x, y);
    if (x == AncestorType::width())
        return ComputeAverageOnRow(pMatrix, y);
    if (x == AncestorType::width() + 1)
        return ComputeMinOnRow(pMatrix, y);
    if (x == AncestorType::width() + 2)
        return ComputeMaxOnRow(pMatrix, y);
    return 0.;
}

wxString ATimeSeries::columnCaption(int colIndx) const
{
    if (colIndx == AncestorType::width())
        return wxT("  Average  ");
    if (colIndx == AncestorType::width() + 1)
        return wxT("   Min.  ");
    if (colIndx == AncestorType::width() + 2)
        return wxT("   Max.  ");
    return AncestorType::columnCaption(colIndx);
}

wxColour ATimeSeries::verticalBorderColor(int x, int y) const
{
    return (x == AncestorType::width() - 1) ? Default::BorderHighlightColor()
                                            : IRenderer::verticalBorderColor(x, y);
}

wxColour ATimeSeries::horizontalBorderColor(int x, int y) const
{
    // Getting informations about the next hour
    // (because the returned color is about the bottom border of the cell,
    // so the next hour for the user)
    if (!(!study) && y + 1 < Date::Calendar::maxHoursInYear)
    {
        auto& hourinfo = study->calendar.hours[y + 1];

        if (hourinfo.firstHourInMonth)
            return Default::BorderMonthSeparator();
        if (hourinfo.firstHourInDay)
            return Default::BorderDaySeparator();
    }
    return IRenderer::verticalBorderColor(x, y);
}

IRenderer::CellStyle ATimeSeries::cellStyle(int col, int row) const
{
    // All timeseries must have a positive value
    double v = cellNumericValue(col, row);

    // Average
    if (col == AncestorType::width())
        return IRenderer::cellStyleAverage;
    // Min
    if (col == AncestorType::width() + 1)
        return IRenderer::cellStyleMinMax;
    // Max
    if (col == AncestorType::width() + 2)
        return IRenderer::cellStyleMinMax;
    // Default
    if (Math::Zero(v))
    {
        if (row % 2)
            return IRenderer::cellStyleDefaultAlternateDisabled;
        else
            return IRenderer::cellStyleDefaultDisabled;
    }
    else
    {
        if (row % 2)
            return IRenderer::cellStyleDefaultAlternate;
        else
            return IRenderer::cellStyleDefault;
    }
}

// =========================
// Clusters ...
// =========================

// ----------------------
//   CLUSTER COMMON
// ----------------------

TimeSeriesCluster::TimeSeriesCluster(wxWindow* control) : AncestorType(control)
{
}

TimeSeriesCluster::~TimeSeriesCluster()
{
    // Disconnect all entities attached to this object
    destroyBoundEvents();
    // Make sure the matrix is no longer available
    this->matrix(nullptr);
}

wxString TimeSeriesCluster::columnCaption(int colIndx) const
{
    if (colIndx == AncestorType::width())
        return wxT("Average");
    if (colIndx == AncestorType::width() + 1)
        return wxT("Min");
    if (colIndx == AncestorType::width() + 2)
        return wxT("Max");
    return AncestorType::columnCaption(colIndx);
}

wxString TimeSeriesCluster::cellValue(int x, int y) const
{
    if (x < AncestorType::width())
        return AncestorType::cellValue(x, y);
    if (x == AncestorType::width())
        return DoubleToWxString(Math::Round(ComputeAverageOnRow(pMatrix, y), 2));
    if (x == AncestorType::width() + 1)
        return DoubleToWxString(ComputeMinOnRow(pMatrix, y));
    if (x == AncestorType::width() + 2)
        return DoubleToWxString(ComputeMaxOnRow(pMatrix, y));
    return wxT("0");
}

double TimeSeriesCluster::cellNumericValue(int x, int y) const
{
    if (x < AncestorType::width())
        return AncestorType::cellNumericValue(x, y);
    if (x == AncestorType::width())
        return ComputeAverageOnRow(pMatrix, y);
    if (x == AncestorType::width() + 1)
        return ComputeMinOnRow(pMatrix, y);
    if (x == AncestorType::width() + 2)
        return ComputeMaxOnRow(pMatrix, y);
    return 0.;
}

IRenderer::CellStyle TimeSeriesCluster::cellStyle(int col, int row) const
{
    // All timeseries must have a positive value
    double v = cellNumericValue(col, row);

    // Average
    if (col == AncestorType::width())
        return IRenderer::cellStyleAverage;
    // Min
    if (col == AncestorType::width() + 1)
        return IRenderer::cellStyleMinMax;
    // Max
    if (col == AncestorType::width() + 2)
        return IRenderer::cellStyleMinMax;
    // Default
    if (Math::Zero(v))
    {
        if (row % 2)
            return IRenderer::cellStyleDefaultAlternateDisabled;
        else
            return IRenderer::cellStyleDefaultDisabled;
    }
    else
    {
        if (row % 2)
            return IRenderer::cellStyleDefaultAlternate;
        else
            return IRenderer::cellStyleDefault;
    }
}

wxColour TimeSeriesCluster::verticalBorderColor(int x, int y) const
{
    return (x == AncestorType::width() - 1) ? Default::BorderHighlightColor()
                                            : IRenderer::verticalBorderColor(x, y);
}

wxColour TimeSeriesCluster::horizontalBorderColor(int x, int y) const
{
    // Getting informations about the next hour
    // (because the returned color is about the bottom border of the cell,
    // so the next hour for the user)
    if (!(!study) && y + 1 < Date::Calendar::maxHoursInYear)
    {
        auto& hourinfo = study->calendar.hours[y + 1];

        if (hourinfo.firstHourInMonth)
            return Default::BorderMonthSeparator();
        if (hourinfo.firstHourInDay)
            return Default::BorderDaySeparator();
    }
    return IRenderer::verticalBorderColor(x, y);
}

// ----------------------
//   THERMAL CLUSTERS
// ----------------------

TimeSeriesThermalCluster::TimeSeriesThermalCluster(
  wxWindow* control,
  Toolbox::InputSelector::ThermalCluster* notifier) :
 TimeSeriesCluster(control)
{
    if (notifier)
        notifier->onThermalClusterChanged.connect(
          this, &TimeSeriesThermalCluster::internalThermalClusterChanged);
}

TimeSeriesThermalCluster::~TimeSeriesThermalCluster()
{
}

void TimeSeriesThermalCluster::onStudyClosed()
{
    internalThermalClusterChanged(nullptr);
    AncestorType::onStudyClosed();
}

// ----------------------
//   Fuel Cost
// ----------------------

TimeSeriesThermalClusterFuelCost::TimeSeriesThermalClusterFuelCost(
  wxWindow* control,
  Toolbox::InputSelector::ThermalCluster* notifier) :
 TimeSeriesCluster(control)
{
    if (notifier)
        notifier->onThermalClusterChanged.connect(
          this, &TimeSeriesThermalClusterFuelCost::internalThermalClusterChanged);
}

TimeSeriesThermalClusterFuelCost::~TimeSeriesThermalClusterFuelCost()
{
}

void TimeSeriesThermalClusterFuelCost::onStudyClosed()
{
    internalThermalClusterChanged(nullptr);
    AncestorType::onStudyClosed();
}

void TimeSeriesThermalClusterFuelCost::internalThermalClusterChanged(
  Antares::Data::ThermalCluster* cluster)
{
    matrix((CurrentStudyIsValid() && cluster) ? &(cluster->ecoInput.fuelcost) : nullptr);
}

// ----------------------
//   CO2 Cost
// ----------------------

TimeSeriesThermalClusterCO2Cost::TimeSeriesThermalClusterCO2Cost(
  wxWindow* control,
  Toolbox::InputSelector::ThermalCluster* notifier) :
 TimeSeriesCluster(control)
{
    if (notifier)
        notifier->onThermalClusterChanged.connect(
          this, &TimeSeriesThermalClusterCO2Cost::internalThermalClusterChanged);
}

TimeSeriesThermalClusterCO2Cost::~TimeSeriesThermalClusterCO2Cost()
{
}

void TimeSeriesThermalClusterCO2Cost::onStudyClosed()
{
    internalThermalClusterChanged(nullptr);
    AncestorType::onStudyClosed();
}

void TimeSeriesThermalClusterCO2Cost::internalThermalClusterChanged(
  Antares::Data::ThermalCluster* cluster)
{
    matrix((CurrentStudyIsValid() && cluster) ? &(cluster->ecoInput.co2cost) : nullptr);
}

// ----------------------
//   RENEWABLE CLUSTERS
// ----------------------

TimeSeriesRenewableCluster::TimeSeriesRenewableCluster(
  wxWindow* control,
  Toolbox::InputSelector::RenewableCluster* notifier) :
 TimeSeriesCluster(control)
{
    if (notifier)
        notifier->onClusterChanged.connect(
          this, &TimeSeriesRenewableCluster::internalRenewableClusterChanged);
}

TimeSeriesRenewableCluster::~TimeSeriesRenewableCluster()
{
}

void TimeSeriesRenewableCluster::onStudyClosed()
{
    internalRenewableClusterChanged(nullptr);
    AncestorType::onStudyClosed();
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
