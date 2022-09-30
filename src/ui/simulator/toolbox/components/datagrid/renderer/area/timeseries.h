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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_TIMESERIES_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_TIMESERIES_H__

#include <antares/wx-wrapper.h>
#include "../area.h"
#include "../matrix.h"
#include "../../../../input/thermal-cluster.h"
#include "../../../../input/renewable-cluster.h"
#include <antares/date.h>
#include "../../../../../application/study.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
class ATimeSeries : public Renderer::Matrix<double, Yuni::sint32>, public Renderer::ARendererArea
{
public:
    using AncestorType = Renderer::Matrix<double, Yuni::sint32>;

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
    using AncestorType = Renderer::Matrix<double, Yuni::sint32>;

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
        matrix((area && Data::Study::Current::Valid()) ? &(area->load.series->series) : NULL);
        Renderer::ARendererArea::internalAreaChanged(area);
    }
};

class TimeSeriesSolar final : public ATimeSeries
{
public:
    using AncestorType = Renderer::Matrix<double, Yuni::sint32>;

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
        matrix((area && Data::Study::Current::Valid()) ? &(area->solar.series->series) : NULL);
        Renderer::ARendererArea::internalAreaChanged(area);
    }
};

class TimeSeriesWind final : public ATimeSeries
{
public:
    using AncestorType = Renderer::Matrix<double, Yuni::sint32>;

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
        matrix((area && Data::Study::Current::Valid()) ? &(area->wind.series->series) : NULL);
        Renderer::ARendererArea::internalAreaChanged(area);
    }
};

class TimeSeriesHydroFatal final : public ATimeSeries
{
public:
    using AncestorType = Renderer::Matrix<double, Yuni::sint32>;

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
        matrix((area && Data::Study::Current::Valid()) ? &(area->hydro.series->ror) : NULL);
        Renderer::ARendererArea::internalAreaChanged(area);
    }
};

class TimeSeriesHydroMod final : public ATimeSeries
{
public:
    using AncestorType = Renderer::Matrix<double, Yuni::sint32>;

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
        matrix((area && Data::Study::Current::Valid()) ? &(area->hydro.series->storage) : NULL);
        Renderer::ARendererArea::internalAreaChanged(area);
    }
};


class TimeSeriesHydroMinGen final : public ATimeSeries
{
public:
    using AncestorType = Renderer::Matrix<double, Yuni::sint32>;

public:
    TimeSeriesHydroMinGen(wxWindow* control, Toolbox::InputSelector::Area* notifier) :
     ATimeSeries(control, notifier)
    {
    }
    virtual ~TimeSeriesHydroMinGen()
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
        matrix((area && Data::Study::Current::Valid()) ? &(area->hydro.series->mingen) : NULL);
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
        matrix((Data::Study::Current::Valid() && cluster) ? &(cluster->series->series) : NULL);
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
    virtual ~TimeSeriesThermalClusterFuelCost();

protected:
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
    virtual ~TimeSeriesThermalClusterCO2Cost();

protected:
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
        matrix((Data::Study::Current::Valid() && cluster) ? &(cluster->series->series) : NULL);
    }

    void onStudyClosed() override;
};

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_TIMESERIES_H__
