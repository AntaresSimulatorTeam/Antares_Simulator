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
