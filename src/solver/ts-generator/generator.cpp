#include "generator.h"

namespace Antares::TSGenerator
{

void ResizeGeneratedTimeSeries(Data::AreaList& areas, Data::Parameters& params)
{
    areas.each([&](Data::Area& area)
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
            area.hydro.series->resize_ROR_STORAGE_MINGEN_whenGeneratedTS(params.nbTimeSeriesHydro);
        }

        // Thermal
        bool globalThermalTSgeneration = params.timeSeriesToGenerate & Data::timeSeriesThermal;
        for (auto [_, cluster] : area.thermal.list.mapping)
        {
            if (cluster->doWeGenerateTS(globalThermalTSgeneration))
                cluster->series.timeSeries.reset(params.nbTimeSeriesThermal, HOURS_PER_YEAR);
        }
    });
}

void DestroyAll(Data::Study& study)
{
    Destroy<Data::timeSeriesLoad>(study, (uint)-1);
    Destroy<Data::timeSeriesSolar>(study, (uint)-1);
    Destroy<Data::timeSeriesWind>(study, (uint)-1);
    Destroy<Data::timeSeriesHydro>(study, (uint)-1);
    Destroy<Data::timeSeriesThermal>(study, (uint)-1);
}

} // Antares::TSGenerator