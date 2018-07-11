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
# define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_TIMESERIES_H__

# include <antares/wx-wrapper.h>
# include "../area.h"
# include "../matrix.h"
# include "../../../../input/thermal-cluster.h"
# include <antares/date.h>
# include "../../../../../application/study.h"



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
		typedef Renderer::Matrix<double, Yuni::sint32> AncestorType;

	public:
		ATimeSeries(wxWindow* control, Toolbox::InputSelector::Area* notifier);

		virtual ~ATimeSeries();

		virtual int width() const {return AncestorType::width() + 3;}
		virtual int height() const {return AncestorType::height();}

		virtual int internalWidth() const {return AncestorType::width();}
		virtual int internalHeight() const {return AncestorType::height();}

		virtual wxString columnCaption(int colIndx) const;

		virtual wxString rowCaption(int rowIndx) const {return AncestorType::rowCaption(rowIndx);}

		virtual wxString cellValue(int x, int y) const;

		virtual double cellNumericValue(int x, int y) const;

		virtual bool cellValue(int x, int y, const Yuni::String& value) {return AncestorType::cellValue(x,y,value);}

		virtual void resetColors(int, int, wxColour&, wxColour&) const
		{/*Do nothing*/}

		virtual wxColour verticalBorderColor(int x, int y) const;
		virtual wxColour horizontalBorderColor(int x, int y) const;

		virtual IRenderer::CellStyle cellStyle(int col, int row) const;

		virtual bool valid() const {return AncestorType::valid();}

		virtual Date::Precision precision() {return Date::hourly;}

	protected:
		//! Event: the study has been closed
		virtual void onStudyClosed() override;
		//! Event: the study has been loaded
		virtual void onStudyLoaded() override;

	}; // class ATimeSeries








	class TimeSeriesLoad final : public ATimeSeries
	{
	public:
		typedef Renderer::Matrix<double, Yuni::sint32> AncestorType;

	public:
		TimeSeriesLoad(wxWindow* control, Toolbox::InputSelector::Area* notifier)
			:ATimeSeries(control, notifier)
		{}
		virtual ~TimeSeriesLoad() {destroyBoundEvents();}

		virtual Date::Precision precision() {return Date::hourly;}

		virtual wxString rowCaption(int rowIndx) const {return AncestorType::rowCaption(rowIndx);}

		virtual uint maxHeightResize() const {return HOURS_PER_YEAR;}

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
		typedef Renderer::Matrix<double, Yuni::sint32> AncestorType;

	public:
		TimeSeriesSolar(wxWindow* control, Toolbox::InputSelector::Area* notifier)
			:ATimeSeries(control, notifier)
		{}
		virtual ~TimeSeriesSolar() {destroyBoundEvents();}

		virtual Date::Precision precision() {return Date::hourly;}

		virtual wxString rowCaption(int rowIndx) const {return AncestorType::rowCaption(rowIndx);}

		virtual uint maxHeightResize() const {return HOURS_PER_YEAR;}

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
		typedef Renderer::Matrix<double, Yuni::sint32> AncestorType;

	public:
		TimeSeriesWind(wxWindow* control, Toolbox::InputSelector::Area* notifier)
			:ATimeSeries(control, notifier)
		{}
		virtual ~TimeSeriesWind() {destroyBoundEvents();}

		virtual Date::Precision precision() {return Date::hourly;}

		virtual wxString rowCaption(int rowIndx) const {return AncestorType::rowCaption(rowIndx);}

		virtual uint maxHeightResize() const {return HOURS_PER_YEAR;}

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
		typedef Renderer::Matrix<double, Yuni::sint32> AncestorType;

	public:
		TimeSeriesHydroFatal(wxWindow* control, Toolbox::InputSelector::Area* notifier)
			:ATimeSeries(control, notifier)
		{}
		virtual ~TimeSeriesHydroFatal() {destroyBoundEvents();}

		virtual Date::Precision precision() {return Date::hourly;}

		virtual uint maxHeightResize() const {return HOURS_PER_YEAR;}

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
		typedef Renderer::Matrix<double, Yuni::sint32> AncestorType;

	public:
		TimeSeriesHydroMod(wxWindow* control, Toolbox::InputSelector::Area* notifier)
			:ATimeSeries(control, notifier)
		{}
		virtual ~TimeSeriesHydroMod() {destroyBoundEvents();}

		virtual Date::Precision precision() {return Date::monthly;}

		virtual wxString rowCaption(int rowIndx) const {return wxStringFromUTF8(Date::MonthToString(rowIndx));}

		virtual uint maxHeightResize() const {return 12;}

	protected:
		virtual void internalAreaChanged(Antares::Data::Area* area)
		{
			matrix((area && Data::Study::Current::Valid()) ? &(area->hydro.series->storage) : NULL);
			Renderer::ARendererArea::internalAreaChanged(area);
		}
	};





	class TimeSeriesThermalCluster final : public Renderer::Matrix<double, yint32>
	{
	public:
		typedef Renderer::Matrix<double, Yuni::sint32> AncestorType;

	public:
		TimeSeriesThermalCluster(wxWindow* control, Toolbox::InputSelector::ThermalCluster* notifier);

		virtual ~TimeSeriesThermalCluster();

		virtual int width() const {return AncestorType::width() + 3;}
		virtual int height() const {return AncestorType::height();}

		virtual int internalWidth() const {return AncestorType::width();}
		virtual int internalHeight() const {return AncestorType::height();}

		virtual wxString columnCaption(int colIndx) const;

		virtual wxString rowCaption(int rowIndx) const {return AncestorType::rowCaption(rowIndx);}

		virtual wxString cellValue(int x, int y) const;

		virtual double cellNumericValue(int x, int y) const;

		virtual bool cellValue(int x, int y, const Yuni::String& value) {return AncestorType::cellValue(x,y,value);}

		virtual void resetColors(int, int, wxColour&, wxColour&) const
		{/*Do nothing*/}

		IRenderer::CellStyle cellStyle(int col, int row) const;

		virtual wxColour verticalBorderColor(int x, int y) const;
		virtual wxColour horizontalBorderColor(int x, int y) const;

		virtual uint maxHeightResize() const {return HOURS_PER_YEAR;}

		virtual Date::Precision precision() {return Date::hourly;}


	protected:
		virtual void internalThermalClusterChanged(Antares::Data::ThermalCluster* cluster)
		{
			matrix( (Data::Study::Current::Valid() && cluster) ? &(cluster->series->series) : NULL);
		}
		virtual void onStudyClosed();
	};







} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_TIMESERIES_H__
