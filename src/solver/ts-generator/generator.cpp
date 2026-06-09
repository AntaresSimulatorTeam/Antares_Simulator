// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/ts-generator/generator.h"

namespace Antares::TSGenerator
{

void ResizeGeneratedTimeSeries(Data::AreaList& areas, Data::Parameters& params)
{
    areas.each(
      [&params](Data::Area& area)
      {
          // Load
          if (params.timeSeriesToGenerate & Data::timeSeriesLoad)
          {
              area.load.series.timeSeries.reset(params.nbTimeSeriesLoad, HOURS_PER_YEAR);
          }

          // Wind
          if (params.timeSeriesToGenerate & Data::timeSeriesWind)
          {
              area.wind.series.timeSeries.reset(params.nbTimeSeriesWind, HOURS_PER_YEAR);
          }

          // Solar
          if (params.timeSeriesToGenerate & Data::timeSeriesSolar)
          {
              area.solar.series.timeSeries.reset(params.nbTimeSeriesSolar, HOURS_PER_YEAR);
          }

          // Hydro
          if (params.timeSeriesToGenerate & Data::timeSeriesHydro)
          {
              area.hydro.series->resizeTS(params.nbTimeSeriesHydro);
          }

          // Thermal
          bool globalThermalTSgeneration = params.timeSeriesToGenerate & Data::timeSeriesThermal;
          for (const auto& cluster: area.thermal.list.all())
          {
              if (cluster->doWeGenerateTS(globalThermalTSgeneration))
              {
                  cluster->series.timeSeries.reset(params.nbTimeSeriesThermal, HOURS_PER_YEAR);
              }
          }
      });
}

void DestroyAll(Data::Study& study)
{
    Destroy<Data::timeSeriesLoad>(study);
    Destroy<Data::timeSeriesSolar>(study);
    Destroy<Data::timeSeriesWind>(study);
    Destroy<Data::timeSeriesHydro>(study);
    Destroy<Data::timeSeriesThermal>(study);
}

} // namespace Antares::TSGenerator
