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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_TIMESERIES_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_TIMESERIES_H__

#include "../area.h"
#include "../matrix.h"
#include "../../../../input/thermal-cluster.h"
#include "../../../../input/renewable-cluster.h"
#include <antares/date/date.h>
#include "../../../../../application/study.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
class ATimeSeries : public Renderer::Matrix<double>, public Renderer::ARendererArea
{
public:
    using AncestorType = Renderer::Matrix<double>;

public:
    ATimeSeries(wxWindow* control, Toolbox::InputSelector::Area* notifier);

    virtual ~ATimeSeries();

    virtual int width() const
    {
        return AncestorType::width() + 3;
    }

    virtual int height() const
    {
        return AncestorType::height();
    }

    virtual int internalWidth() const
    {
        return AncestorType::width();
    }
    virtual int internalHeight() const
    {
        return AncestorType::height();
    }

    virtual wxString columnCaption(int colIndx) const;

    virtual wxString rowCaption(int rowIndx) const
    {
        return AncestorType::rowCaption(rowIndx);
    }

    virtual wxString cellValue(int x, int y) const;

    virtual double cellNumericValue(int x, int y) const;

    virtual bool cellValue(int x, int y, const Yuni::String& value)
    {
        return AncestorType::cellValue(x, y, value);
    }

    virtual void resetColors(int, int, wxColour&, wxColour&) const
    { /*Do nothing*/
    }

    virtual wxColour verticalBorderColor(int x, int y) const;
    virtual wxColour horizontalBorderColor(int x, int y) const;

    virtual IRenderer::CellStyle cellStyle(int col, int row) const;

    virtual bool valid() const
    {
        return AncestorType::valid();
    }

    virtual Date::Precision precision()
    {
        return Date::hourly;
    }

protected:
    //! Event: the study has been closed
    virtual void onStudyClosed() override;
    //! Event: the study has been loaded
    virtual void onStudyLoaded() override;

}; // class ATimeSeries

class TimeSeriesLoad final : public ATimeSeries
{
public:
    using AncestorType = Renderer::Matrix<double>;

public:
    TimeSeriesLoad(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
     ATimeSeries(control, notifier)
    {
    }
    virtual ~TimeSeriesLoad()
    {
        destroyBoundEvents();
    }

    virtual Date::Precision precision()
    {
        return Date::hourly;
    }

    virtual wxString rowCaption(int rowIndx) const
    {
        return AncestorType::rowCaption(rowIndx);
    }

    virtual uint maxHeightResize() const
    {
        return HOURS_PER_YEAR;
    }

protected:
    virtual void internalAreaChanged(Antares::Data::Area* area)
    {
        matrix((area && CurrentStudyIsValid()) ? &(area->load.series.timeSeries) : NULL);
        Renderer::ARendererArea::internalAreaChanged(area);
    }
};

class TimeSeriesSolar final : public ATimeSeries
{
public:
    using AncestorType = Renderer::Matrix<double>;

public:
    TimeSeriesSolar(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
     ATimeSeries(control, notifier)
    {
    }
    virtual ~TimeSeriesSolar()
    {
        destroyBoundEvents();
    }

    virtual Date::Precision precision()
    {
        return Date::hourly;
    }

    virtual wxString rowCaption(int rowIndx) const
    {
        return AncestorType::rowCaption(rowIndx);
    }

    virtual uint maxHeightResize() const
    {
        return HOURS_PER_YEAR;
    }

protected:
    virtual void internalAreaChanged(Antares::Data::Area* area)
    {
        matrix((area && CurrentStudyIsValid()) ? &(area->solar.series.timeSeries) : NULL);
        Renderer::ARendererArea::internalAreaChanged(area);
    }
};

class TimeSeriesWind final : public ATimeSeries
{
public:
    using AncestorType = Renderer::Matrix<double>;

public:
    TimeSeriesWind(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
     ATimeSeries(control, notifier)
    {
    }
    virtual ~TimeSeriesWind()
    {
        destroyBoundEvents();
    }

    virtual Date::Precision precision()
    {
        return Date::hourly;
    }

    virtual wxString rowCaption(int rowIndx) const
    {
        return AncestorType::rowCaption(rowIndx);
    }

    virtual uint maxHeightResize() const
    {
        return HOURS_PER_YEAR;
    }

protected:
    virtual void internalAreaChanged(Antares::Data::Area* area)
    {
        matrix((area && CurrentStudyIsValid()) ? &(area->wind.series.timeSeries) : NULL);
        Renderer::ARendererArea::internalAreaChanged(area);
    }
};

class TimeSeriesHydroFatal final : public ATimeSeries
{
public:
    using AncestorType = Renderer::Matrix<double>;

public:
    TimeSeriesHydroFatal(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
     ATimeSeries(control, notifier)
    {
    }
    virtual ~TimeSeriesHydroFatal()
    {
        destroyBoundEvents();
    }

    virtual Date::Precision precision()
    {
        return Date::hourly;
    }

    virtual uint maxHeightResize() const
    {
        return HOURS_PER_YEAR;
    }

protected:
    virtual void internalAreaChanged(Antares::Data::Area* area)
    {
        matrix((area && CurrentStudyIsValid()) ? &(area->hydro.series->ror.timeSeries) : NULL);
        Renderer::ARendererArea::internalAreaChanged(area);
    }
};

class TimeSeriesHydroMod final : public ATimeSeries
{
public:
    using AncestorType = Renderer::Matrix<double>;

public:
    TimeSeriesHydroMod(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
     ATimeSeries(control, notifier)
    {
    }
    virtual ~TimeSeriesHydroMod()
    {
        destroyBoundEvents();
    }

    virtual Date::Precision precision()
    {
        return Date::daily;
    }

    virtual wxString rowCaption(int rowIndx) const
    {
        return wxStringFromUTF8(study->calendar.text.daysYear[rowIndx]);
    }

    virtual uint maxHeightResize() const
    {
        return DAYS_PER_YEAR;
    }

protected:
    virtual void internalAreaChanged(Antares::Data::Area* area)
    {
        matrix((area && CurrentStudyIsValid()) ? &(area->hydro.series->storage.timeSeries) : NULL);
        Renderer::ARendererArea::internalAreaChanged(area);
    }
};


class TimeSeriesHydroMinGen final : public ATimeSeries
{
public:
    using AncestorType = Renderer::Matrix<double>;

    TimeSeriesHydroMinGen(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
     ATimeSeries(control, notifier)
    {
    }
    ~TimeSeriesHydroMinGen() override
    {
        destroyBoundEvents();
    }

    Date::Precision precision() override
    {
        return Date::hourly;
    }

    uint maxHeightResize() const override
    {
        return HOURS_PER_YEAR;
    }

private:
    void internalAreaChanged(Antares::Data::Area* area) override
    {
        matrix((area && CurrentStudyIsValid()) ? &(area->hydro.series->mingen.timeSeries) : NULL);
        Renderer::ARendererArea::internalAreaChanged(area);
    }
};

class TimeSeriesHydroMaxHourlyGenPower final : public ATimeSeries
{
public:
    using AncestorType = Renderer::Matrix<double, int32_t>;

    TimeSeriesHydroMaxHourlyGenPower(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
     ATimeSeries(control, notifier)
    {
    }
    ~TimeSeriesHydroMaxHourlyGenPower() override
    {
        destroyBoundEvents();
    }

    Date::Precision precision() override
    {
        return Date::hourly;
    }

    uint maxHeightResize() const override
    {
        return HOURS_PER_YEAR;
    }

private:
    void internalAreaChanged(Antares::Data::Area* area) override
    {
        matrix((area && CurrentStudyIsValid()) ? &(area->hydro.series->maxHourlyGenPower.timeSeries) : NULL);
        Renderer::ARendererArea::internalAreaChanged(area);
    }
};

class TimeSeriesHydroMaxHourlyPumpPower final : public ATimeSeries
{
public:
    using AncestorType = Renderer::Matrix<double, int32_t>;

    TimeSeriesHydroMaxHourlyPumpPower(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
     ATimeSeries(control, notifier)
    {
    }
    ~TimeSeriesHydroMaxHourlyPumpPower() override
    {
        destroyBoundEvents();
    }

    Date::Precision precision() override
    {
        return Date::hourly;
    }

    uint maxHeightResize() const override
    {
        return HOURS_PER_YEAR;
    }

private:
    void internalAreaChanged(Antares::Data::Area* area) override
    {
        matrix((area && CurrentStudyIsValid()) ? &(area->hydro.series->maxHourlyPumpPower.timeSeries) : NULL);
        Renderer::ARendererArea::internalAreaChanged(area);
    }
};

// =========================
// Clusters ...
// =========================

// ----------------------
//   CLUSTER COMMON
// ----------------------

class TimeSeriesCluster : public Renderer::Matrix<double>
{
public:
    using AncestorType = Renderer::Matrix<double>;

public:
    TimeSeriesCluster(wxWindow* control);
    virtual ~TimeSeriesCluster();

    int width() const override
    {
        return AncestorType::width() + 3;
    }

    int height() const override
    {
        return AncestorType::height();
    }

    int internalWidth() const override
    {
        return AncestorType::width();
    }
    int internalHeight() const override
    {
        return AncestorType::height();
    }

    wxString columnCaption(int colIndx) const override;

    wxString rowCaption(int rowIndx) const override
    {
        return AncestorType::rowCaption(rowIndx);
    }

    wxString cellValue(int x, int y) const override;

    double cellNumericValue(int x, int y) const override;

    void resetColors(int, int, wxColour&, wxColour&) const override
    {
        // Do nothing
    }

    bool cellValue(int x, int y, const Yuni::String& value) override
    {
        return AncestorType::cellValue(x, y, value);
    }

    IRenderer::CellStyle cellStyle(int col, int row) const override;

    wxColour verticalBorderColor(int x, int y) const override;
    wxColour horizontalBorderColor(int x, int y) const override;

    uint maxHeightResize() const override
    {
        return HOURS_PER_YEAR;
    }

    Date::Precision precision() override
    {
        return Date::hourly;
    }
};

// ----------------------
//   THERMAL CLUSTERS
// ----------------------

class TimeSeriesThermalCluster final : public TimeSeriesCluster
{
public:
    TimeSeriesThermalCluster(wxWindow* control, Toolbox::InputSelector::ThermalCluster* notifier);

    virtual ~TimeSeriesThermalCluster();

protected:
    void internalThermalClusterChanged(Antares::Data::ThermalCluster* cluster)
    {
        matrix((CurrentStudyIsValid() && cluster) ? &(cluster->series.timeSeries) : NULL);
    }

    void onStudyClosed() override;
};

// ----------------------
//   Fuel Cost
// ----------------------
class TimeSeriesThermalClusterFuelCost final : public TimeSeriesCluster
{
public:
    TimeSeriesThermalClusterFuelCost(wxWindow* control, Toolbox::InputSelector::ThermalCluster* notifier);
    ~TimeSeriesThermalClusterFuelCost() override;

private:
    void internalThermalClusterChanged(Antares::Data::ThermalCluster* cluster);
    void onStudyClosed() override;
};

// ----------------------
//   CO2 Cost
// ----------------------
class TimeSeriesThermalClusterCO2Cost final : public TimeSeriesCluster
{
public:
    TimeSeriesThermalClusterCO2Cost(wxWindow* control, Toolbox::InputSelector::ThermalCluster* notifier);
    ~TimeSeriesThermalClusterCO2Cost() override;

private:
    void internalThermalClusterChanged(Antares::Data::ThermalCluster* cluster);
    void onStudyClosed() override;
};

// ----------------------
//   RENEWABLE CLUSTERS
// ----------------------

class TimeSeriesRenewableCluster final : public TimeSeriesCluster
{
public:
    TimeSeriesRenewableCluster(wxWindow* control,
                               Toolbox::InputSelector::RenewableCluster* notifier);

    virtual ~TimeSeriesRenewableCluster();

private:
    void internalRenewableClusterChanged(Antares::Data::RenewableCluster* cluster)
    {
        matrix((CurrentStudyIsValid() && cluster) ? &(cluster->series.timeSeries) : NULL);
    }

    void onStudyClosed() override;
};

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_TIMESERIES_H__
