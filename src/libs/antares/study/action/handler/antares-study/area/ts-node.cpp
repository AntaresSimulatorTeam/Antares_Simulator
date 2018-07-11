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

#include "ts-node.h"
#include "../../../../fwd.h"


using namespace Yuni;


namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Area
{


	NodeTimeseries::NodeTimeseries(Data::TimeSeries ts) :
		pType(ts)
	{
		// behavior
		pInfos.behavior = bhOverwrite;

		switch (ts)
		{
			case Data::timeSeriesLoad:
				pInfos.caption << "Load";
				break;
			case Data::timeSeriesSolar:
				pInfos.caption << "Solar";
				break;
			case Data::timeSeriesWind:
				pInfos.caption << "Wind";
				break;
			case Data::timeSeriesHydro:
				pInfos.caption << "Hydro";
				break;
			case Data::timeSeriesThermal:
				pInfos.caption << "Thermal";
				break;
			default:
				break;
		}
	}


	NodeTimeseries::~NodeTimeseries()
	{}


	bool NodeTimeseries::prepareWL(Context&)
	{
		pInfos.message.clear();
		pInfos.state = stReady;
		switch (pInfos.behavior)
		{
			case bhOverwrite:
				pInfos.message = "The timeseries and the local data will be reset to default";
				break;
			default:
				pInfos.state = stNothingToDo;
		}
		return true;
	}


	bool NodeTimeseries::performWL(Context& ctx)
	{
		if (!ctx.area)
			return false;
		switch (pType)
		{
			case Data::timeSeriesLoad:
				ctx.area->load.resetToDefault();
				break;
			case Data::timeSeriesSolar:
				ctx.area->solar.resetToDefault();
				break;
			case Data::timeSeriesWind:
				ctx.area->wind.resetToDefault();
				break;
			case Data::timeSeriesHydro:
				ctx.area->hydro.intraDailyModulation = 2.;
				ctx.area->hydro.interDailyBreakdown  = 0.;
				assert(ctx.area->hydro.series);
				ctx.area->hydro.series->reset();
				assert(ctx.area->hydro.prepro);
				ctx.area->hydro.prepro->reset();
				break;
			case Data::timeSeriesThermal:
				if (ctx.cluster)
				{
					ctx.cluster->series->series.reset(1, HOURS_PER_YEAR);
					ctx.cluster->prepro->reset();
				}
				break;
			default:
				break;
		}
		return true;
	}





} // namespace Area
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares

