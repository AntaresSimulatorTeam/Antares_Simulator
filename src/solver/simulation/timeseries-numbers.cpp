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



























#include <yuni/yuni.h>
#include "timeseries-numbers.h"
#include <antares/study.h>
#include <antares/study/scenario-builder/sets.h>
#include "../aleatoire/alea_fonctions.h"

using namespace Yuni;





# define TS_INDEX(T) \
	Data::TimeSeriesBitPatternIntoIndex<T>::value





# define DRAW_A_RANDOM_NUMBER(T,X,PreproSize)  \
	 \
	((!intramodal[TS_INDEX(T)]) \
		 \
		? ((uint32)(floor((double)(study.runtime->random[Data::seedTimeseriesNumbers].next() \
			* (tsgen[TS_INDEX(T)] ? PreproSize : X.width))))) \
		 \
		: draw[TS_INDEX(T)])







# define CORRELATION_CHECK_AND_INIT(T) \
	do \
	{ \
		if (intramodal[TS_INDEX(T)] && !tsgen[TS_INDEX(T)]) \
		{ \
			if (!CorrelationCheck<T>(study, nbTimeseries[TS_INDEX(T)]))  \
				return false; \
		} \
		else \
		{ \
			switch (T) \
			{ \
				case Data::timeSeriesLoad:    nbTimeseries[TS_INDEX(T)] = parameters.nbTimeSeriesLoad;break; \
				case Data::timeSeriesSolar:   nbTimeseries[TS_INDEX(T)] = parameters.nbTimeSeriesSolar;break; \
				case Data::timeSeriesWind:    nbTimeseries[TS_INDEX(T)] = parameters.nbTimeSeriesWind;break; \
				case Data::timeSeriesHydro:   nbTimeseries[TS_INDEX(T)] = parameters.nbTimeSeriesHydro;break; \
				case Data::timeSeriesThermal: nbTimeseries[TS_INDEX(T)] = parameters.nbTimeSeriesThermal;break; \
				case Data::timeSeriesCount: break; \
			} \
		} \
	} \
	while (0)



# define CORRELATION_CHECK_INTERMODAL_SINGLE_AREA(T, PREPRO_WIDTH, MTX_WIDTH) \
	do \
	{ \
		const unsigned int tsindx = TS_INDEX(T); \
		if (intermodal[tsindx]) \
		{ \
			if (1 != (w = (tsgen[tsindx] ? PREPRO_WIDTH : MTX_WIDTH))) \
			{ \
				if (r[tsindx] != 1 && r[tsindx] != w) \
				{ \
					logs.error() << "Inter-modal correlation: Constraint violation: The number of time-series for '" \
						<< area.name << "' does not match (found " << w << ", expected " << r[tsindx] << ')'; \
					return false; \
				} \
				r[tsindx] = w; \
			} \
		} \
	} \
	while (0)



# define BUILD_LOG_ENTRY(T, TEXT) \
		do \
		{ \
			if (intermodal[TS_INDEX(T)]) \
			{ \
				if (first) \
				{ \
					e += ", "; \
					first = false; \
				} \
				e += TEXT; \
			} \
		} \
		while (0)





namespace Antares
{
namespace Solver
{


	


	static void ApplyCustomTSNumbers(Data::Study& study)
	{
		
		auto& parameters = study.parameters;

		auto& rulename = parameters.activeRulesScenario;
		logs.info() << "Preparing time-series numbers... (" << rulename << ')';
		logs.info() << "  :: Scenario Builder, active target: " << rulename;
		Data::RulesScenarioName id = rulename;
		id.toLower();

		
		study.scenarioRulesLoadIfNotAvailable();
		if (study.scenarioRules)
		{
			Data::ScenarioBuilder::Rules::Ptr rules = study.scenarioRules->find(id);
			if (!(!rules))
			{
				
				rules->apply(study);
			}
			else
				logs.error() << "Scenario Builder: Impossible to find the active ruleset '" << rulename << "'";
		}

		
		study.scenarioRulesDestroy();
		logs.info(); 
	}


	template<enum Data::TimeSeries TimeSeriesT>
	static unsigned int CheckMatricesWidth(const Data::Study& study)
	{
		unsigned int w = 1;
		auto end = study.areas.end();
		for (auto i = study.areas.begin(); i != end; ++i)
		{
			const Data::Area& area = *(i->second);
			switch (TimeSeriesT)
			{
				case Data::timeSeriesLoad:
					{
						if (area.load.series->series.width != w)
						{
							if (area.load.series->series.width != 1 && w != 1)
								return 0;
							if (area.load.series->series.width > 1)
								w = area.load.series->series.width;
						}
						break;
					}
				case Data::timeSeriesSolar:
					{
						if (area.solar.series->series.width != w)
						{
							if (area.solar.series->series.width != 1 && w != 1)
								return 0;
							if (area.solar.series->series.width > 1)
								w = area.solar.series->series.width;
						}
						break;
					}
				case Data::timeSeriesWind:
					{
						if (area.wind.series->series.width != w)
						{
							if (area.wind.series->series.width != 1 && w != 1)
								return 0;
							if (area.wind.series->series.width > 1)
								w = area.wind.series->series.width;
						}
						break;
					}
				case Data::timeSeriesHydro:
					{
						if (area.hydro.series->count != w)
						{
							if (area.hydro.series->count != 1 && w != 1)
								return 0;
							if (area.hydro.series->count > 1)
								w = area.hydro.series->count;
						}
						break;
					}
				case Data::timeSeriesThermal:
					{
						unsigned int clusterCount = area.thermal.clusterCount;
						for (unsigned int i = 0; i != clusterCount; ++i)
						{
							
							auto& cluster = *(area.thermal.clusters[i]);

							if (cluster.series->series.width != w)
							{
								if (cluster.series->series.width != 1 && w != 1)
									return 0;
								if (cluster.series->series.width > 1)
									w = cluster.series->series.width;
							}
						}
						break;
					}
			}
		}
		return w;
	}



	template<enum Data::TimeSeries TimeSeriesT>
	static bool
	CorrelationCheck(const Data::Study& study, unsigned int& nbTimeseries)
	{
		switch (TimeSeriesT)
		{
			case Data::timeSeriesLoad:
				logs.info() << "Checking intra-modal correlation: Load";
				break;
			case Data::timeSeriesSolar:
				logs.info() << "Checking intra-modal correlation: Solar";
				break;
			case Data::timeSeriesWind:
				logs.info() << "Checking intra-modal correlation: Wind";
				break;
			case Data::timeSeriesHydro:
				logs.info() << "Checking intra-modal correlation: Hydro";
				break;
			case Data::timeSeriesThermal:
				logs.info() << "Checking intra-modal correlation: Thermal";
				break;
			case Data::timeSeriesCount:
				break;
		}
		nbTimeseries = CheckMatricesWidth<TimeSeriesT>(study);
		if (!nbTimeseries)
		{
			logs.error() << "Intra-modal correlation: Constraint violation: The number of time-series must be identical for all areas";
			return false;
		}
		return true;
	}


	static void StoreTimeseriesIntoOuput(Data::Study& study)
	{
		using namespace Antares::Data;
		
		if (study.parameters.storeTimeseriesNumbers)
		{
			study.storeTimeSeriesNumbers<timeSeriesLoad>();
			study.storeTimeSeriesNumbers<timeSeriesSolar>();
			study.storeTimeSeriesNumbers<timeSeriesHydro>();
			study.storeTimeSeriesNumbers<timeSeriesWind>();
			study.storeTimeSeriesNumbers<timeSeriesThermal>();
		}
	}




	


	static bool GenerateDeratedMode(Data::Study& study)
	{
		logs.info() <<  "  :: using the `derated` mode";
		if (study.parameters.useCustomTSNumbers)
			logs.warning() << "The derated mode is enabled. The custom building mode will be ignored";

		study.areas.each([&] (Data::Area& area)
		{
			area.load  .series->timeseriesNumbers.zero();
			area.solar .series->timeseriesNumbers.zero();
			area.wind  .series->timeseriesNumbers.zero();
			area.hydro .series->timeseriesNumbers.zero();

			
			for (unsigned int i = 0; i != area.thermal.clusterCount; ++i)
			{
				auto& cluster = *(area.thermal.clusters[i]);
				cluster.series->timeseriesNumbers.zero();
			}
		});

		
		StoreTimeseriesIntoOuput(study);
		return true;
	}







	bool TimeSeriesNumbers::Generate(Data::Study& study)
	{
		
		logs.info() << "Preparing time-series numbers...";

		
		
		
		
		
		
		
		
		

		
		auto& parameters = study.parameters;

		
		if (parameters.derated)
			return GenerateDeratedMode(study);

		
		const unsigned int years = 1 + study.runtime->rangeLimits.year[Data::rangeEnd];

		
		const bool intramodal[Data::timeSeriesCount] =
		{
			0 != (Data::timeSeriesLoad    & parameters.intraModal),
			0 != (Data::timeSeriesHydro   & parameters.intraModal),
			0 != (Data::timeSeriesWind    & parameters.intraModal),
			0 != (Data::timeSeriesThermal & parameters.intraModal),
			0 != (Data::timeSeriesSolar   & parameters.intraModal)
		};
		const bool intermodal[Data::timeSeriesCount] =
		{
			0 != (Data::timeSeriesLoad    & parameters.interModal),
			0 != (Data::timeSeriesHydro   & parameters.interModal),
			0 != (Data::timeSeriesWind    & parameters.interModal),
			0 != (Data::timeSeriesThermal & parameters.interModal),
			0 != (Data::timeSeriesSolar   & parameters.interModal)
		};

		
		unsigned int nbTimeseries[Data::timeSeriesCount];
		
		uint32 draw[Data::timeSeriesCount] = {0, 0, 0, 0, 0};
		
		const bool tsgen[Data::timeSeriesCount] =
		{
			0 != (Data::timeSeriesLoad    & parameters.timeSeriesToRefresh),
			0 != (Data::timeSeriesHydro   & parameters.timeSeriesToRefresh),
			0 != (Data::timeSeriesWind    & parameters.timeSeriesToRefresh),
			0 != (Data::timeSeriesThermal & parameters.timeSeriesToRefresh),
			0 != (Data::timeSeriesSolar   & parameters.timeSeriesToRefresh)
		};

		
		CORRELATION_CHECK_AND_INIT(Data::timeSeriesLoad);
		CORRELATION_CHECK_AND_INIT(Data::timeSeriesHydro);
		CORRELATION_CHECK_AND_INIT(Data::timeSeriesWind);
		CORRELATION_CHECK_AND_INIT(Data::timeSeriesThermal);
		CORRELATION_CHECK_AND_INIT(Data::timeSeriesSolar);

		
		for (unsigned int y = 0; y < years; ++y)
		{
			
			for (unsigned int z = 0; z < Data::timeSeriesCount; ++z)
			{
				if (intramodal[z])
				{
					draw[z] = (uint32)(floor(study.runtime->random[Data::seedTimeseriesNumbers].next()
						* nbTimeseries[z]));
				}
			}

			
			study.areas.each([&] (Data::Area& area)
			{
				
				assert(y < area.load.series->timeseriesNumbers.height);
				area.load.series->timeseriesNumbers[0][y] =
					DRAW_A_RANDOM_NUMBER(Data::timeSeriesLoad, area.load.series->series,
					parameters.nbTimeSeriesLoad);

				
				assert(y < area.solar.series->timeseriesNumbers.height);
				area.solar.series->timeseriesNumbers[0][y] =
					DRAW_A_RANDOM_NUMBER(Data::timeSeriesSolar, area.solar.series->series,
					parameters.nbTimeSeriesSolar);

				
				assert(y < area.wind.series->timeseriesNumbers.height);
				area.wind.series->timeseriesNumbers[0][y] =
					DRAW_A_RANDOM_NUMBER(Data::timeSeriesWind, area.wind.series->series,
					parameters.nbTimeSeriesWind);

				
				assert(y < area.hydro.series->timeseriesNumbers.height);
				area.hydro.series->timeseriesNumbers[0][y] =
					DRAW_A_RANDOM_NUMBER(Data::timeSeriesHydro, area.hydro.series->ror,
					parameters.nbTimeSeriesHydro);

				
				auto end = area.thermal.list.mapping.end();
				for (auto i = area.thermal.list.mapping.begin(); i != end; ++i)
				{
					auto* cluster = i->second;
					if (!cluster->enabled)
					{
						
						study.runtime->random[Data::seedTimeseriesNumbers].next();
					}
					else
					{
						cluster->series->timeseriesNumbers.entry[0][y] =
							DRAW_A_RANDOM_NUMBER(Data::timeSeriesThermal, cluster->series->series,
							parameters.nbTimeSeriesThermal);
					}
				}
				











			}); 
		} 

		
		if (parameters.interModal)
		{
			{
				CString<248, false> e = "Checking inter-modal correlation... (";
				bool first = true;
				BUILD_LOG_ENTRY(Data::timeSeriesLoad,    "load");
				BUILD_LOG_ENTRY(Data::timeSeriesSolar,   "solar");
				BUILD_LOG_ENTRY(Data::timeSeriesWind,    "wind");
				BUILD_LOG_ENTRY(Data::timeSeriesHydro,   "hydro");
				BUILD_LOG_ENTRY(Data::timeSeriesThermal, "thermal");
				logs.info() << e << ')';
			}
			
			unsigned int w;

			
			auto end = study.areas.end();
			for (auto i = study.areas.begin(); i != end; ++i)
			{
				
				auto& area = *(i->second);
				unsigned int r[Data::timeSeriesCount] = {1, 1, 1, 1, 1};

				CORRELATION_CHECK_INTERMODAL_SINGLE_AREA(Data::timeSeriesLoad, parameters.nbTimeSeriesLoad,
					area.load.series->series.width);
				CORRELATION_CHECK_INTERMODAL_SINGLE_AREA(Data::timeSeriesSolar, parameters.nbTimeSeriesSolar,
					area.solar.series->series.width);
				CORRELATION_CHECK_INTERMODAL_SINGLE_AREA(Data::timeSeriesWind, parameters.nbTimeSeriesWind,
					area.wind.series->series.width);
				CORRELATION_CHECK_INTERMODAL_SINGLE_AREA(Data::timeSeriesHydro, parameters.nbTimeSeriesHydro,
					area.hydro.series->count);

				if (intermodal[TS_INDEX(Data::timeSeriesThermal)])
				{
					
					const unsigned int clusterCount = area.thermal.clusterCount;
					for (unsigned int j = 0; j != clusterCount; ++j)
					{
						auto& cluster = *(area.thermal.clusters[j]);
						CORRELATION_CHECK_INTERMODAL_SINGLE_AREA(Data::timeSeriesThermal, parameters.nbTimeSeriesThermal,
							cluster.series->series.width);
					}
				}

				
				unsigned int q = 1;
				for (unsigned int j = 0; j != Data::timeSeriesCount; ++j)
				{
					if (r[j] != 1)
					{
						if (q != 1 && q != r[j])
						{
							logs.error() << "Inter-modal correlation: Constraint violation: The number of time-series for '"
								<< area.name << "' does not match (found " << r[j] << ", expected " << q << ')';
							return false;
						}
						q = r[j];
					}
				}

				
				
				
				
				Matrix<uint32>* tsNumbers = nullptr;
				if (intermodal[TS_INDEX(Data::timeSeriesLoad)])
					tsNumbers = &(area.load.series->timeseriesNumbers);
				else
				{
					if (intermodal[TS_INDEX(Data::timeSeriesSolar)])
						tsNumbers = &(area.solar.series->timeseriesNumbers);
					else
						if (intermodal[TS_INDEX(Data::timeSeriesWind)])
							tsNumbers = &(area.wind.series->timeseriesNumbers);
						else
							if (intermodal[TS_INDEX(Data::timeSeriesHydro)])
								tsNumbers = &(area.hydro.series->timeseriesNumbers);
				}
				assert(tsNumbers);

				for (unsigned int y = 0; y < years; ++y)
				{
					const unsigned int draw = tsNumbers->entry[0][y];
					assert(draw < 100000);

					
					assert(y < area.load.series->timeseriesNumbers.height);
					if (intermodal[TS_INDEX(Data::timeSeriesLoad)])
						area.load.series->timeseriesNumbers.entry[0][y] = draw;

					
					assert(y < area.solar.series->timeseriesNumbers.height);
					if (intermodal[TS_INDEX(Data::timeSeriesSolar)])
						area.solar.series->timeseriesNumbers.entry[0][y] = draw;

					
					assert(y < area.wind.series->timeseriesNumbers.height);
					if (intermodal[TS_INDEX(Data::timeSeriesWind)])
						area.wind.series->timeseriesNumbers.entry[0][y] = draw;

					
					assert(y < area.hydro.series->timeseriesNumbers.height);
					if (intermodal[TS_INDEX(Data::timeSeriesHydro)])
						area.hydro.series->timeseriesNumbers.entry[0][y] = draw;

					
					if (intermodal[TS_INDEX(Data::timeSeriesThermal)])
					{
						unsigned int clusterCount = area.thermal.clusterCount;
						for (unsigned int i = 0; i != clusterCount; ++i)
						{
							auto& cluster = *(area.thermal.clusters[i]);
							assert(y < cluster.series->timeseriesNumbers.height);
							cluster.series->timeseriesNumbers.entry[0][y] = draw;
						}
					}
				}
			}
		}

		
		if (parameters.useCustomTSNumbers)
			ApplyCustomTSNumbers(study);

		
		StoreTimeseriesIntoOuput(study);

		return true;
	}





} 
} 

